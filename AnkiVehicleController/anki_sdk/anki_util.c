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

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <assert.h>

void bytes_to_hex(const void *value, size_t len, char **output) {
    const unsigned char *bytes = (const unsigned char *)value;
    size_t bufferLen = len * 3 * sizeof(char);
    char str[bufferLen];
    memset(str, 0, bufferLen);
  
    const char *hex = "0123456789ABCDEF";
    char *s = str;
  
    for (size_t i = 0; i < len-1; ++i ) {
        *s++ = hex[(*bytes>>4)&0xF];
        *s++ = hex[(*bytes++)&0xF];
        *s++ = ' ';
    }
    *s++ = hex[(*bytes>>4)&0xF];
    *s++ = hex[(*bytes++)&0xF];
    *s++ = 0;

    if (output != NULL)
        memmove(*output, str, bufferLen); 
}

void hexdump(const char *prefix, const size_t column_len, const void *value, size_t len) {
    assert(prefix != NULL);
    assert(column_len > 0);
    int row_count = (len / column_len) + 1;
    int i;

    size_t slen = column_len*3;
    char s[slen];
    for (i = 0; i < row_count; ++i) {
        memset(s, 0, column_len*3);
        bytes_to_hex(value, len, (char **)&s);
        printf("%s %s\n", prefix, s);
    }
}


