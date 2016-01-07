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

#ifndef INCLUDE_advertisement_h
#define INCLUDE_advertisement_h

#include <stdint.h>
#include "common.h"
#include "uuid.h"

ANKI_BEGIN_DECL

/**
 * The state of a vehicle recorded in the advertising packet.
 *
 * - full_battery: The vehicle battery is fully charged
 * - low_battery: The vehicle battery has a low charge and will die soon 
 * - on_charger: The vehicle is currently on the charger
 */
typedef struct anki_vehicle_adv_state {
  uint8_t _reserved:4;
  uint8_t full_battery:1;      // 4: TRUE if Car has full battery
  uint8_t low_battery:1;       // 5: TRUE if Car has low battery
  uint8_t on_charger:1;        // 6: TRUE if Car is on Charger
  uint8_t _unavailable:1;    // 7: Cannot be used due to BTLE character set constraint
} anki_vehicle_adv_state_t;

/**
 * Vehicle information packed in the LOCAL_NAME string record
 * of an advertising packet.
 *
 * - state: Current vehicle state.
 *   NOTE: Changes to the vehicle state will cause the LOCAL_NAME value
 *   to change.
 * - version: Firmware version running on the vehicle
 * - name: User-defined name in UTF-8 encoding
 */
typedef struct anki_vehicle_adv_info {
    anki_vehicle_adv_state_t state;
    uint16_t            version;
    uint8_t             _reserved[5];
    unsigned char       name[13]; // UTF8: 12 bytes + NULL.
} anki_vehicle_adv_info_t;


/**
 * Vehicle hardware information encodeded in the MANUFACTURER_DATA
 * record of an advertising packet.
 *
 * - identifier: Unique identifer for a physical vehicle
 * - model_id: The model type of a vehicle
 * - product_id: Value identifying the vehicle as Anki Drive hardware
 */
typedef struct anki_vehicle_adv_mfg {
    uint32_t    identifier;
    uint8_t     model_id;
    uint8_t     _reserved;
    uint16_t    product_id;
} anki_vehicle_adv_mfg_t;


/**
 * Vehicle information present in Bluetooth LE advertising packets.
 *
 * flags: EIR / AD flags
 * tx_power: transmission power
 * mfg_data: parsed data from the MANUFACTURER_DATA bytes
 * local_name: parsed data from the LOCAL_NAME string bytes
 * service_id: Anki Vehicle UUID (128-bit)
 */
typedef struct anki_vehicle_adv {
    uint8_t                     flags;
    uint8_t                     tx_power;
    anki_vehicle_adv_mfg_t      mfg_data;
    anki_vehicle_adv_info_t     local_name;
    uuid128_t                   service_id;
} anki_vehicle_adv_t;

/**
 * Test whether the scan record contains an Anki Drive vehicle service UUID.
 *
 * @param scan_data Bytes obtained by scanning vehicle advertising packets.
 * @param scan_data_len Length of bytes in scan_data.
 *
 * @return 0 on failure, 1 on success.
 */
uint8_t anki_vehicle_adv_record_has_anki_uuid(const uint8_t *scan_data, const size_t scan_data_len);


/**
 * Parse advertising data for an Anki Drive vehicle.
 *
 * @param scan_data Bytes obtained by scanning vehicle advertising packets.
 * @param scan_data_len Length of bytes in scan_data.
 * @param anki_vehicle_adv Pointer to a structure for storing accumulated information for a vehicle.
 * Parsed scan data will overwrite matching record data in the struct.
 *
 * @return  0 on success, 1 on failure,
 *          2 if the data is not anki drive 
 *          3 if unknown EIR data is encountered
 *
 * @discuss Pass NULL for anki_vehicle_adv in order to determine if this is an appropriate record.
 *
 * 
 */
uint8_t anki_vehicle_parse_adv_record(const uint8_t *scan_data, const size_t scan_data_len, anki_vehicle_adv_t *anki_vehicle_adv);

/**
 * Parse MANUFACTERER_DATA record from a vehicle advertising packet.
 *
 * @param scan_data Bytes obtained by scanning vehicle advertising packets.
 * @param scan_data_len Length of bytes in scan_data.
 * @param mfg_data Pointer to a structure for storing parsed data.
 * 
 * @return 0 on success, 1 on failure.
 */
uint8_t anki_vehicle_parse_mfg_data(const uint8_t *bytes, uint8_t len, anki_vehicle_adv_mfg_t *mfg_data);

/**
 * Parse LOCAL_NAME record from a vehicle advertising packet.
 *
 * @param scan_data Bytes obtained by scanning vehicle advertising packets.
 * @param scan_data_len Length of bytes in scan_data.
 * @param local_name Pointer to a structure for storing parsed data.
 *
 * @return 0 on success, 1 on failure.
 */
uint8_t anki_vehicle_parse_local_name(const uint8_t *bytes, uint8_t len, anki_vehicle_adv_info_t *local_name);

ANKI_END_DECL

#endif
