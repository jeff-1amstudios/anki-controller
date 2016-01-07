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

#ifndef INCLUDE_eir_h
#define INCLUDE_eir_h

#include <stdint.h>
#include <stddef.h>

#include "common.h"

ANKI_BEGIN_DECL

#define BLE_ADV_RECORD_MAX_LEN  32
struct ble_adv_record {
    uint8_t type;
    uint8_t length;
    uint8_t data[30];
};
typedef struct ble_adv_record ble_adv_record_t;

/*
 * These Advertising Data types are required for parsing Anki Drive vehicle records.
 *
 * This is an incomplete list.
 * See https://www.bluetooth.org/en-us/specification/assigned-numbers/generic-access-profile
 * for all supported values
 */ 
enum ble_adv_record_type {
    ADV_TYPE_INVALID = 0,
    ADV_TYPE_FLAGS = 0x1,
    ADV_TYPE_UUID_128 = 0x7,
    ADV_TYPE_LOCAL_NAME = 0x9,
    ADV_TYPE_TX_POWER = 0xa,
    ADV_TYPE_MANUFACTURER_DATA = 0xff
};
typedef uint8_t ble_adv_record_type_t;

int ble_adv_parse_scan(const uint8_t *data, const size_t data_len, size_t *record_count, ble_adv_record_t records[]);

ANKI_END_DECL

#endif
