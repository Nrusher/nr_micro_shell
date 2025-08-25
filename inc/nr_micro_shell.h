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

#ifndef __NR_MICRO_SHELL_H
#define __NR_MICRO_SHELL_H

#ifdef __cplusplus
extern "C" {
#endif

#include "nr_micro_shell_port.h"

#define NR_SHELL_VERSION "2.0.0"

/* Default config */
#ifndef NR_SHELL_MAX_LINE_SZ
#define NR_SHELL_MAX_LINE_SZ 80
#endif

#ifndef NR_SHELL_PROMPT
#define NR_SHELL_PROMPT "nr@dev"
#endif

#ifndef NR_SHELL_MAX_PARAM_NUM
#define NR_SHELL_MAX_PARAM_NUM 8
#endif

#ifdef NR_SHELL_HISTORY_CMD_SUPPORT
#ifndef NR_SHELL_HISTORY_CMD_NUM
#define NR_SHELL_HISTORY_CMD_NUM 5
#endif

#ifndef NR_SHELL_HISTORY_CMD_SZ
#define NR_SHELL_HISTORY_CMD_SZ 50
#endif
#endif

struct nr_micro_shell {
	uint8_t cursor_pos;
	uint8_t rcv_len;
	char line[NR_SHELL_MAX_LINE_SZ];
	uint8_t state;

#ifdef NR_SHELL_HISTORY_CMD_SUPPORT
	char his_cmd[NR_SHELL_HISTORY_CMD_NUM][NR_SHELL_HISTORY_CMD_SZ];
	char shadow_line[NR_SHELL_MAX_LINE_SZ];
	uint8_t his_cmd_count;
	uint8_t his_cmd_request_index;
	uint8_t his_cmd_rear;
#endif
};

struct cmd {
	char *name;
	int (*func)(uint8_t argc, char **argv);
	char *desc;
};

extern struct cmd cmd_table[];
extern char *auto_complete_words[];
extern const uint16_t cmd_table_size;
extern const uint16_t auto_complete_words_size;

void shell_printf(const char *fmt, ...);
void shell_init(void);
void shell(char c);
void show_all_cmds(void);

#ifdef __cplusplus
}
#endif
#endif /* __NR_MICRO_SHELL_H */

/******************* (C) COPYRIGHT 2025 Ji Youzhou ****************************/