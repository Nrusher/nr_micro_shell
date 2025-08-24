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

#include "time.h"
#include "stdint.h"
#include "nr_micro_shell_port.h"
#include "nr_micro_shell.h"

FILE *dbug_log;
FILE *key_rec_log;

uint64_t get_sys_timestamp(void)
{
	struct timespec ts;
	clock_gettime(CLOCK_MONOTONIC, &ts);
	return ts.tv_sec * 1000000000 + ts.tv_nsec;
}

void nr_shell_debug_log_init(void)
{
	dbug_log = fopen("debug.log", "w");
	if (!dbug_log)
		printf("failed to open debug log\n");
	setvbuf(dbug_log, NULL, _IONBF, 0);
	key_rec_log = fopen("key_rec.log", "w");
	if (!key_rec_log)
		printf("failed to open key rec log\n");
	setvbuf(key_rec_log, NULL, _IONBF, 0);
}

/******************* (C) COPYRIGHT 2025 Ji Youzhou ****************************/
