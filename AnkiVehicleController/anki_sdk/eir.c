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

int ble_adv_parse_scan(const uint8_t *data, const size_t data_len, size_t *record_count, ble_adv_record_t records[])
{
    // no data to parse
    if (data == NULL)
        return 1;

    uint8_t record_index = 0;
    int i = 0;
    while (i < data_len) {
        uint8_t len = data[i++];
        if (len == 0)   break;

        ble_adv_record_type_t type = data[i];
        if (type == ADV_TYPE_INVALID)   break;

        if (records != NULL) {
            ble_adv_record_t *record = &records[record_index];
            record->type = type;
            record->length = (len-1);
            memmove(record->data, &data[i+1], len-1);
        }
        
        record_index++;
        i += len; 
    }

    if (record_count != NULL) {
        *record_count = record_index;
    }

    return 0;
}
