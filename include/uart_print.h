/*
 * Copyright 2019 Ravikiran Bukkasagara <contact@ravikiranb.com>
 *        
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *    
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#ifndef UART_PRINT_H_
#define UART_PRINT_H_

#include <stdio.h>
#include <string.h>

enum {
    DEBUG_LEVEL_NONE,
    DEBUG_LEVEL_ERROR,
    DEBUG_LEVEL_INFO,
    DEBUG_LEVEL_DEBUG,
};

#define DEBUG_LEVEL DEBUG_LEVEL_INFO

#define CONSOLE_PRINT_BUF_SIZE 128

extern char console_print_buf[];

int console_init(void);
void console_prints(const char* str);
int console_fprintf(FILE* stream, const char* fmt, ...);

#define dbg_printf(level, ...)                                                \
    if ((level) <= DEBUG_LEVEL) {                                             \
        snprintf(console_print_buf, CONSOLE_PRINT_BUF_SIZE - 1, __VA_ARGS__); \
        console_prints(console_print_buf);                                    \
    }

#endif /* UART_PRINT_H_ */
