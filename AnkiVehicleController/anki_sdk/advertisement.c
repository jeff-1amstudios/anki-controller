/*
 * Copyright (c) 2014 Anki, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "eir.h"
#include "advertisement.h"
#include "vehicle_gatt_profile.h"
#include "uuid.h"
#include "anki_util.h"

#define VEHICLE_STATE_FULL_BATTERY  (1 << 4)
#define VEHICLE_STATE_LOW_BATTERY   (1 << 5)
#define VEHICLE_STATE_ON_CHARGER    (1 << 6)
#define IS_VEHICLE_STATE_SET(state, flag) ( ((state) & (flag)) == (flag) )

int is_anki_vehicle_service_uuid(uuid128_t *a);

uint8_t anki_vehicle_parse_adv_record(const uint8_t *scan_data, const size_t scan_data_len, anki_vehicle_adv_t *anki_vehicle_adv)
{
    // no data to parse
    if (scan_data == NULL)
        return 1;

    int i = 0;
    while (i < scan_data_len) {
        uint8_t len = scan_data[i++];
        if (len == 0)   break;

        ble_adv_record_type_t type = scan_data[i];
        if (type == ADV_TYPE_INVALID)   break;

        const uint8_t *data = &scan_data[i+1];
        uint8_t data_len = len - 1; 
        switch(type) {
            case ADV_TYPE_INVALID:
                return -1;
                break;
            case ADV_TYPE_FLAGS:
                if (anki_vehicle_adv != NULL)
                    memmove(&anki_vehicle_adv->flags, data, data_len);
                break;
            case ADV_TYPE_UUID_128:
            {
                if ((data_len == sizeof(uuid128_t)) &&
                    !is_anki_vehicle_service_uuid((uuid128_t*)data) )
                    return 2;

                if (anki_vehicle_adv != NULL) {
                    memmove(&anki_vehicle_adv->service_id, data, data_len);
                } else {
                    return 0;
                }
                break;
            }
            case ADV_TYPE_LOCAL_NAME:
                if (anki_vehicle_adv != NULL) {
                    anki_vehicle_parse_local_name(data, data_len, &anki_vehicle_adv->local_name);
                }
                break;
            case ADV_TYPE_TX_POWER:
                if (anki_vehicle_adv != NULL)
                    memmove(&anki_vehicle_adv->tx_power, data, data_len);
                break;
            case ADV_TYPE_MANUFACTURER_DATA:
                if (anki_vehicle_adv != NULL) {
                    anki_vehicle_parse_mfg_data(data, data_len, &anki_vehicle_adv->mfg_data);
                } 
                break;
            default:
                return 3;
                printf("\nunknown type: %d\n", type);
                hexdump("data: ", 80, data, data_len);
                break;
        }

        i += len;
    }

    return ((anki_vehicle_adv != NULL) && is_anki_vehicle_service_uuid(&anki_vehicle_adv->service_id)) ? 0 : 2;
}

uint8_t anki_vehicle_adv_record_has_anki_uuid(const uint8_t *scan_data, const size_t scan_data_len)
{
    return (anki_vehicle_parse_adv_record(scan_data, scan_data_len, NULL) == 0);
}

int is_anki_vehicle_service_uuid(uuid128_t *a)
{
    static const uint8_t ANKI_SERVICE_UUID_BYTES[16] = ANKI_SERVICE_UUID_LE;
    return (uuid128_cmp(a, (uuid128_t *)ANKI_SERVICE_UUID_BYTES) == 0); 
}

uint8_t anki_vehicle_parse_mfg_data(const uint8_t *bytes, uint8_t len, anki_vehicle_adv_mfg_t *mfg_data)
{
    if (bytes == NULL || len != 8) return 1;

    // bytes stored on the vehicle in Big Endian order
    // This assumes the host is little endian
    
    mfg_data->product_id = (bytes[1] | (bytes[0] << 8)); 
    mfg_data->_reserved = bytes[2];
    mfg_data->model_id = bytes[3];
    mfg_data->identifier = (bytes[7] | (bytes[6] << 8) | (bytes[5] << 16) | (bytes[4] << 24));

    return 0;
}

uint8_t anki_vehicle_parse_local_name(const uint8_t *bytes, uint8_t len, anki_vehicle_adv_info_t *local_name)
{
    if (len == 0)   return 1;

    uint8_t state = (uint8_t)bytes[0] & 0xff;
    local_name->state.full_battery = IS_VEHICLE_STATE_SET(state, VEHICLE_STATE_FULL_BATTERY);
    local_name->state.low_battery = IS_VEHICLE_STATE_SET(state, VEHICLE_STATE_LOW_BATTERY);
    local_name->state.on_charger = IS_VEHICLE_STATE_SET(state, VEHICLE_STATE_ON_CHARGER);

    if (len > 1) {
        local_name->version = bytes[1];
    }

    if (len > 2) {
        local_name->version |= (bytes[2] << 8);
    }

    if (len > 8) {
        uint8_t name_len = len - 8;
        memset(local_name->name, 0, (sizeof local_name->name));
        memmove(local_name->name, &bytes[8], name_len);
    }

    return 0;
}
