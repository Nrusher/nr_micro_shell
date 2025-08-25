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

#include "nr_micro_shell.h"
#include "stdint.h"
#include "stdio.h"
#include "stdlib.h"

int cmd_help(uint8_t argc, char **argv)
{
	show_all_cmds();
	return 0;
}

#ifdef NR_SHELL_CMD_RD
int cmd_rd(uint8_t argc, char **argv)
{
	uint32_t addr, size = 256;
	uint8_t *p;
	int i, j;

	if (argc < 2) {
		shell_printf("Usage: rd <addr> [size]\n");
		return -1;
	}

	addr = strtoul(argv[1], NULL, 16);
	if (sscanf(argv[1], "%x", &addr) != 1) {
		shell_printf("Invalid address format\n");
		return -1;
	}

	if (argc > 2) {
		if (sscanf(argv[2], "%x", &size) != 1) {
			printf("Invalid size format\n");
			return -1;
		}
	}

	size = (size + 15) & ~15;

	p = (uint8_t *)(uintptr_t)addr;
	for (i = 0; i < size; i += 16) {
		shell_printf("%08x: ", (uint32_t)(uintptr_t)(p + i));
		for (j = 0; j < 16; j++) {
			if (i + j < size) {
				shell_printf("%02x ", p[i + j]);
			} else {
				shell_printf("   ");
			}
		}

		shell_printf(" ");
		for (j = 0; j < 16; j++) {
			if (i + j < size) {
				if (p[i + j] >= 32 && p[i + j] <= 126) {
					shell_printf("%c", p[i + j]);
				} else {
					shell_printf(".");
				}
			} else {
				shell_printf(" ");
			}
		}

		shell_printf("\n");
	}

	return 0;
}
#endif

#ifdef NR_SHELL_CMD_WR
int cmd_wr(uint8_t argc, char **argv)
{
	uint32_t addr;
	uint32_t value;
	uint8_t *p;
	int size = 4;

	if (argc < 3) {
		shell_printf("Usage: wr <addr> <value> [b|w|l]\n");
		shell_printf("  b: byte (1 byte)\n");
		shell_printf("  w: word (2 bytes)\n");
		shell_printf("  l: long (4 bytes, default)\n");
		return -1;
	}

	if (sscanf(argv[1], "%x", (uint32_t *)&addr) != 1) {
		shell_printf("Invalid address format\n");
		return -1;
	}

	if (sscanf(argv[2], "%x", &value) != 1) {
		shell_printf("Invalid value format\n");
		return -1;
	}

	if (argc > 3) {
		switch (argv[3][0]) {
			case 'b': size = 1; break;
			case 'w': size = 2; break;
			case 'l': size = 4; break;
			default: shell_printf("Invalid size format, use b/w/l\n"); return -1;
		}
	}

	p = (uint8_t *)(uintptr_t)addr;

	switch (size) {
		case 1:
			*p = (uint8_t)value;
			shell_printf("Wrote 0x%02x to 0x%08x\n", value, addr);
			break;
		case 2:
			*(uint16_t *)p = (uint16_t)value;
			shell_printf("Wrote 0x%04x to 0x%08x\n", value, addr);
			break;
		case 4:
			*(uint32_t *)p = value;
			shell_printf("Wrote 0x%08x to 0x%08x\n", value, addr);
			break;
	}

	return 0;
}
#endif

#ifdef NR_SHELL_CMD_HEX2DEC
int cmd_hex2dec(uint8_t argc, char **argv)
{
	uint32_t value;

	if (argc < 2) {
		shell_printf("Usage: hex2dec <hex>\n");
		return -1;
	}

	if (sscanf(argv[1], "%x", &value) != 1) {
		shell_printf("Invalid hex format\n");
		return -1;
	}

	shell_printf("0x%x = %u\n", value, value);
	return 0;
}
#endif

struct cmd cmd_table[] = { { .name = "help", .func = cmd_help, .desc = "show this help" },

#ifdef NR_SHELL_CMD_RD
			   { .name = "rd", .func = cmd_rd, .desc = "read memory" },
#endif

#ifdef NR_SHELL_CMD_WR
			   { .name = "wr", .func = cmd_wr, .desc = "write memory" },
#endif

#ifdef NR_SHELL_CMD_HEX2DEC
			   { .name = "hex2dec", .func = cmd_hex2dec, .desc = "hex to decimal" },
#endif
};

const uint16_t cmd_table_size = sizeof(cmd_table) / sizeof(cmd_table[0]);

#ifdef NR_SHELL_AUTO_COMPLETE_SUPPORT
char *auto_complete_words[] = { "-h", "-p", "-t" };
const uint16_t auto_complete_words_size = sizeof(auto_complete_words) / sizeof(auto_complete_words[0]);
#endif



/******************* (C) COPYRIGHT 2025 Ji Youzhou *********************************/