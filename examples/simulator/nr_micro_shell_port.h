/*
 * *****************************************************************************
 * MIT License
 * 
 * Copyright (C) 2025 Ji Youzhou. or its affiliates.  All Rights Reserved.
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 ***********************************************************************************/

#ifndef __NR_MICRO_SHELL_PORT_H__
#define __NR_MICRO_SHELL_PORT_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdint.h>

/* Required configuration macros */
#define shell_putc(x) putchar((x))
#define NR_SHELL_MAX_LINE_SZ 80
#define NR_SHELL_PROMPT "nr@dev"
#define NR_SHELL_MAX_PARAM_NUM 16

/* Optional configuration macros */

#define NR_SHELL_SHOW_LOGO

#define NR_SHELL_AUTO_COMPLETE_SUPPORT

#define NR_SHELL_HISTORY_CMD_SUPPORT
#define NR_SHELL_HISTORY_CMD_NUM 5
#define NR_SHELL_HISTORY_CMD_SZ 64

uint64_t get_sys_timestamp(void);
#define shell_get_ts() get_sys_timestamp()

#define NR_SHELL_DEBUG

#ifdef NR_SHELL_DEBUG
extern FILE *dbug_log;
extern FILE *key_rec_log;
#define DLOG(fmt, ...) fprintf(dbug_log, "[%ldns] %s %d: " fmt "\n", get_sys_timestamp(), __func__, __LINE__, ##__VA_ARGS__)
#define KEY_RECORD(c) fprintf(key_rec_log, "%x ", c)
void nr_shell_debug_log_init(void);
#endif


#ifdef __cplusplus
}
#endif
#endif /* __NR_MICRO_SHELL_PORT_H__ */
/******************* (C) COPYRIGHT 2025 Ji Youzhou *********************************/