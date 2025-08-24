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

#include "nr_micro_shell_port.h"
#include "stdint.h"
#include "string.h"
#include "stdarg.h"
#include "stdio.h"
#include "nr_micro_shell.h"

#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))
#define NEWLINE "\r\n"
#define NR_SHELL_PROMPT_START_POS (sizeof(NR_SHELL_PROMPT) + 3)

#ifndef NR_SHELL_DEBUG
#define DLOG(...)
#define KEY_RECORD(c)
#define debug_log_init()
#endif

#define BS 0x08
#define HT 0x09
#define LF 0x0A
#define CR 0x0D
#define DEL 0x7F
#define ESC 0x1B
#define CSI 0x9B

static struct nr_micro_shell _default_sh;

#define is_printable_ch(c) ((c) >= 32 && (c) <= 126)

typedef int (*key_func_t)(struct nr_micro_shell *sh, uint8_t c);

enum { ESnormal, ESesc, ESsquare, ESwave };

enum { PRINTABLE_KEY, ENTER, TAB, UP, DOWN, LEFT, RIGHT, BACKSPACE, DELETE, UNKNOWN, MAX_KEY_ID } typedef keycode_t;

void shell_printf(const char *fmt, ...)
{
	va_list args;
	char buffer[NR_SHELL_MAX_LINE_SZ];
	int len;
	char *p;

	va_start(args, fmt);
	len = vsnprintf(buffer, sizeof(buffer), fmt, args);
	va_end(args);

	if (len >= sizeof(buffer)) {
		len = sizeof(buffer) - 1;
	}
	buffer[len] = '\0';

	p = buffer;
	while (*p) {
		shell_putc(*p++);
	}
}

#ifdef NR_SHELL_HISTORY_CMD_SUPPORT
static void add_history_cmd(struct nr_micro_shell *sh)
{
	int i;

	// Don't add empty command to history
	if (!sh->line[0])
		return;

	// Avoid duplicate storage
	if (sh->his_cmd_count > 0) {
		if (!strcmp(sh->line, sh->his_cmd[sh->his_cmd_rear - 1])) {
			return;
		}
	}

	strncpy(sh->his_cmd[sh->his_cmd_rear], sh->line, sh->rcv_len);

	if (sh->his_cmd_count < NR_SHELL_HISTORY_CMD_NUM) {
		sh->his_cmd_count++;
	}

	sh->his_cmd_rear++;
	sh->his_cmd_rear %= NR_SHELL_HISTORY_CMD_NUM;

	DLOG("his_cmd_rear: %d", sh->his_cmd_rear);
	for (i = 0; i < sh->his_cmd_count; i++) {
		DLOG("his_cmd[%d]: %s", i, sh->his_cmd[i]);
	}
}
#endif

static void print_prompt(struct nr_micro_shell *sh)
{
	shell_printf("%s: ", NR_SHELL_PROMPT);
	sh->cursor_pos = 0;
	sh->rcv_len = 0;
	sh->line[0] = '\0';
}

static struct cmd *find_cmd(char *name)
{
	if (!name)
		return NULL;

	for (int i = 0; i < cmd_table_size; i++) {
		if (!strcmp(name, cmd_table[i].name)) {
			return &cmd_table[i];
		}
	}
	return NULL;
}

static int split_cmdline(char *line, uint8_t *argc, char *argv[], uint8_t argv_sz)
{
	char *p = line;
	uint8_t arg_count = 0;
	char *arg_start = NULL;

	while (*p) {
		if (!arg_start && *p == ' ') {
			p++;
			continue;
		}

		if (!arg_start) {
			arg_start = p;
		}

		if (*p == ' ') {
			if (arg_count < argv_sz) {
				*p = '\0';
				argv[arg_count] = arg_start;
				arg_count++;
			} else {
				return -1;
			}
			arg_start = NULL;
		}
		p++;
	}

	if (arg_start && arg_count < argv_sz) {
		argv[arg_count] = arg_start;
		arg_count++;
	}

	*argc = arg_count;

	return 0;
}

static int run_cmdline(struct nr_micro_shell *sh)
{
	int i;
	uint8_t argc = 0;
	char *argv[NR_SHELL_MAX_PARAM_NUM];
	char line_tmp[NR_SHELL_MAX_LINE_SZ];
	struct cmd *cmd = NULL;
	int ret = -1;

	argv[0] = NULL;

	strcpy(line_tmp, sh->line);
	DLOG("line: %s", line_tmp);
	ret = split_cmdline(line_tmp, &argc, argv, NR_SHELL_MAX_PARAM_NUM);

	if (ret < 0) {
		shell_printf("\r\nInput parameter count > %d!\r\n", NR_SHELL_MAX_PARAM_NUM);
		return ret;
	}

	for (i = 0; i < argc; i++) {
		DLOG("argv[%d]: %s", i, argv[i]);
	}

	cmd = find_cmd(argv[0]);

	if (cmd) {
		DLOG("cmd: %s, %s", cmd->name, cmd->desc);
		shell_printf("\r\n");
		if (cmd->func) {
			ret = cmd->func(argc, argv);
		} else {
			shell_printf("cmd %s not implemented.\r\n", cmd->name);
		}

	} else if (argv[0]) {
		shell_printf("\r\ncan't find cmd %s.\r\n", argv[0]);
	} else {
		shell_printf("\r\n");
	}

#ifdef NR_SHELL_HISTORY_CMD_SUPPORT
	if (!ret) {
		add_history_cmd(sh);
	}
#endif

	return ret;
}

#ifdef NR_SHELL_AUTO_COMPLETE_SUPPORT
static int complete(struct nr_micro_shell *sh)
{
	char *cursor = sh->line + sh->cursor_pos;
	char *word_start = cursor;
	char *word_end = cursor;
	int word_len;
	int match_count = 0;
	char *match = NULL;
	int i;

	DLOG("complete: %s, cursor_pos: %d, rcv_len: %d", sh->line, sh->cursor_pos, sh->rcv_len);

	sh->line[sh->rcv_len] = '\0';

	// Find the start and end position of current word
	while (word_start > sh->line && *(word_start - 1) != ' ') {
		word_start--;
	}
	while (*word_end != ' ' && *word_end != '\0') {
		word_end++;
	}
	word_len = word_end - word_start;

	// Don't complete if cursor is at whitespace or after a word
	if (word_len == 0 || (*cursor == ' ' && cursor > sh->line && *(cursor - 1) == ' ')) {
		return 0;
	}

	DLOG("word_start: %d, word_end: %d, word_len: %d", (int)(word_start - sh->line), (int)(word_end - sh->line), word_len);

	// Check if it's the first word (command name)
	if (word_start == sh->line) {
		// Search for matching commands in cmd_table
		for (i = 0; i < cmd_table_size; i++) {
			if (strncmp(word_start, cmd_table[i].name, word_len) == 0) {
				if (match_count == 0) {
					match = cmd_table[i].name;
				} else if (strcmp(match, cmd_table[i].name) != 0) {
					match = NULL; // Multiple different matches found
				}
				match_count++;
			}
		}
	} else {
		// Search for matching words in auto_complete_words
		for (i = 0; i < auto_complete_words_size; i++) {
			if (strncmp(word_start, auto_complete_words[i], word_len) == 0) {
				if (match_count == 0) {
					match = auto_complete_words[i];
				} else if (match && strcmp(match, auto_complete_words[i]) != 0) {
					match = NULL; // Multiple different matches found
				}
				match_count++;
			}
		}
	}

	DLOG("match_count: %d, match: %s", match_count, match);

	if (match_count > 1) {
		// Multiple matches found, show all possible options
		shell_printf("\r\n");
		if (word_start == sh->line) {
			for (i = 0; i < cmd_table_size; i++) {
				if (strncmp(word_start, cmd_table[i].name, word_len) == 0) {
					shell_printf("%s ", cmd_table[i].name);
				}
			}
		} else {
			for (i = 0; i < auto_complete_words_size; i++) {
				if (strncmp(word_start, auto_complete_words[i], word_len) == 0) {
					shell_printf("%s ", auto_complete_words[i]);
				}
			}
		}
		shell_printf("\r\n");
		shell_printf("%s: ", NR_SHELL_PROMPT);
		shell_printf("%s", sh->line);
		if (sh->rcv_len - sh->cursor_pos > 0) {
			shell_printf("\x1b[%dD", sh->rcv_len - sh->cursor_pos); // Move cursor to correct position
		}
	} else if (match_count == 1 && match != NULL) {
		// Single match found, complete it
		int complete_len = strlen(match) - word_len;
		if (complete_len > 0) {
			if (sh->rcv_len + complete_len < NR_SHELL_MAX_LINE_SZ) {
				memmove(word_end, word_end + complete_len, strlen(word_end) + 1);
				memcpy(word_start + word_len, match + word_len, complete_len);
				sh->rcv_len += complete_len;
				sh->cursor_pos += complete_len;
				shell_printf("%s", match + word_len);
			}
		}
	}

	return 0;
}
#endif

static int normal_key_func(struct nr_micro_shell *sh, uint8_t c)
{
	int i;
	if (sh->rcv_len == NR_SHELL_MAX_LINE_SZ - 2) {
		shell_printf("\r\nline too long!\r\n");
		print_prompt(sh);
		return 0;
	}

	if (sh->rcv_len == sh->cursor_pos) {
		sh->line[sh->cursor_pos] = c;
		sh->cursor_pos++;
	} else {
		for (i = sh->rcv_len; i >= sh->cursor_pos; i--) {
			sh->line[i + 1] = sh->line[i];
		}
		sh->line[sh->cursor_pos] = c;
		sh->cursor_pos++;
	}

	sh->rcv_len++;
	sh->line[sh->rcv_len] = '\0';

	if (sh->rcv_len != sh->cursor_pos) {
		shell_printf("\x1b[%d@", 1); // Move cursor to correct position
	}
	shell_putc(c);
	return 0;
};

static int enter_key_func(struct nr_micro_shell *sh, uint8_t c)
{
	int ret;

	DLOG("enter key");
	sh->line[sh->rcv_len] = '\0';
	DLOG("line: %s", sh->line);
	if (sh->rcv_len != 0) {
		ret = run_cmdline(sh);
	} else {
		shell_printf("\r\n");
	}

	print_prompt(sh);

	return ret;
};

void show_all_cmds(void)
{
	int i = 0;
	shell_printf("--------------------------------------------\r\n");
	for (i = 0; i < cmd_table_size; i++) {
		shell_printf("%s\t%s\n", cmd_table[i].name, cmd_table[i].desc);
	}
}

static int tab_key_func(struct nr_micro_shell *sh, uint8_t c)
{
	DLOG("tab key");
	if (sh->rcv_len == 0) {
		shell_printf("\r\n");
		show_all_cmds();
		print_prompt(sh);
	} else {
#ifdef NR_SHELL_AUTO_COMPLETE_SUPPORT
		complete(sh);
#endif
	}

	return 0;
}

static int up_key_func(struct nr_micro_shell *sh, uint8_t c)
{
#ifdef NR_SHELL_HISTORY_CMD_SUPPORT

	uint8_t his_id = 0;
	int i;

	DLOG("up key");

	if (sh->his_cmd_count == 0 || sh->his_cmd_request_index >= sh->his_cmd_count) {
		return 0;
	}

	DLOG("his_cmd_request_index: %d", sh->his_cmd_request_index);
	for (i = 0; i < sh->his_cmd_count; i++) {
		if (i + 1 != sh->his_cmd_rear) {
			DLOG("      his_cmd[%d]: %s", i, sh->his_cmd[i]);
		} else {
			DLOG("last->his_cmd[%d]: %s", i, sh->his_cmd[i]);
		}
	}
	DLOG("shadow_line: %s", sh->shadow_line);

	if (sh->shadow_line[0] == '\0' && sh->his_cmd_request_index == 0) {
		strcpy(sh->shadow_line, sh->line);
		DLOG("store shadow_line: %s", sh->shadow_line);
	}

	sh->his_cmd_request_index++;

	// convert requst index to his_id
	his_id = (sh->his_cmd_rear + NR_SHELL_HISTORY_CMD_NUM - sh->his_cmd_request_index) % NR_SHELL_HISTORY_CMD_NUM;

	DLOG("refill line, his_cmd[%d]: %s, his_cmd_request_index: %d", his_id, sh->his_cmd[his_id], sh->his_cmd_request_index);

	shell_printf("\x1b[2K\r");
	print_prompt(sh);
	strcpy(sh->line, sh->his_cmd[his_id]);
	sh->rcv_len = strlen(sh->line);
	sh->cursor_pos = sh->rcv_len;
	shell_printf("%s", sh->line);
#endif

	return 0;
};

static int down_key_func(struct nr_micro_shell *sh, uint8_t c)
{
#ifdef NR_SHELL_HISTORY_CMD_SUPPORT
	char *tmp;
	uint8_t his_id = 0;
	int i;

	DLOG("down key");

	if (sh->his_cmd_request_index == 0) {
		return 0;
	}

	sh->his_cmd_request_index--;

	DLOG("his_cmd_request_index: %d", sh->his_cmd_request_index);
	for (i = 0; i < sh->his_cmd_count; i++) {
		if (i != sh->his_cmd_rear) {
			DLOG("      his_cmd[%d]: %s", i, sh->his_cmd[i]);
		} else {
			DLOG("rear->his_cmd[%d]: %s", i, sh->his_cmd[i]);
		}
	}
	DLOG("shadow_line: %s", sh->shadow_line);

	if (sh->his_cmd_request_index == 0) {
		tmp = sh->shadow_line;
	} else {
		// convert requst index to his_id
		his_id = (sh->his_cmd_rear + NR_SHELL_HISTORY_CMD_NUM - sh->his_cmd_request_index) % NR_SHELL_HISTORY_CMD_NUM;
		tmp = sh->his_cmd[his_id];
	}

	shell_printf("\x1b[2K\r");
	print_prompt(sh);
	strcpy(sh->line, tmp);
	sh->rcv_len = strlen(sh->line);
	sh->cursor_pos = sh->rcv_len;
	shell_printf("%s", sh->line);
#endif

	return 0;
}

static int right_key_func(struct nr_micro_shell *sh, uint8_t c)
{
	DLOG("right key, cursor_pos: %d, rcv_len: %d, line: %s", sh->cursor_pos, sh->rcv_len, sh->line);
	if (sh->cursor_pos == sh->rcv_len)
		return 0;
	sh->cursor_pos++;
	shell_printf("\x1b[C");
	return 0;
}

static int left_key_func(struct nr_micro_shell *sh, uint8_t c)
{
	DLOG("left key, cursor_pos: %d, rcv_len: %d, line: %s", sh->cursor_pos, sh->rcv_len, sh->line);
	if (sh->cursor_pos == 0)
		return 0;
	sh->cursor_pos--;
	shell_printf("\x1b[D");
	return 0;
}

static int backspace_key_func(struct nr_micro_shell *sh, uint8_t c)
{
	int i;

	DLOG("backspace key, cursor_pos: %d, rcv_len: %d, line: %s", sh->cursor_pos, sh->rcv_len, sh->line);
	if (sh->cursor_pos == 0)
		return 0;
	sh->cursor_pos--;
	sh->rcv_len--;
	for (i = sh->cursor_pos; i < sh->rcv_len; i++) {
		sh->line[i] = sh->line[i + 1];
	}
	shell_printf("\x1b[D\x1b[P");
	return 0;
}

static int delete_key_func(struct nr_micro_shell *sh, uint8_t c)
{
	int i;

	DLOG("delete key, cursor_pos: %d, rcv_len: %d, line: %s", sh->cursor_pos, sh->rcv_len, sh->line);
	if (sh->cursor_pos == sh->rcv_len)
		return 0;
	sh->rcv_len--;
	for (i = sh->cursor_pos; i < sh->rcv_len; i++) {
		sh->line[i] = sh->line[i + 1];
	}
	shell_printf("\x1b[P");
	return 0;
}

static int unknown_key_func(struct nr_micro_shell *sh, uint8_t c)
{
	return 0;
}

static key_func_t key_funcs[MAX_KEY_ID] = { normal_key_func, enter_key_func,  tab_key_func,   up_key_func,
					    down_key_func,   left_key_func,   right_key_func, backspace_key_func,
					    delete_key_func, unknown_key_func };

static int run_key_func(keycode_t x, struct nr_micro_shell *sh, char c)
{
	int ret = -1;
	if (x >= MAX_KEY_ID)
		return 0;

	if (key_funcs[x] == NULL)
		return -1;

#ifdef NR_SHELL_HISTORY_CMD_SUPPORT
	if (x != UP && x != DOWN && sh->state == ESnormal) {
		sh->his_cmd_request_index = 0;
		sh->shadow_line[0] = '\0';
	}
#endif

	ret = key_funcs[x](sh, c);
	return ret;
}

static keycode_t get_keycode(struct nr_micro_shell *sh, uint8_t c)
{
	keycode_t ret = UNKNOWN;

	if (sh->state == ESnormal) {
		if (is_printable_ch(c))
			return PRINTABLE_KEY;
		switch (c) {
			case BS: return BACKSPACE;
			case LF:
			case CR: return ENTER;
			case DEL: return BACKSPACE;
			case HT: return TAB;
			case ESC: sh->state = ESesc; return UNKNOWN;
		}
	}

	switch (sh->state) {
		case ESesc:
			if (c == '[') {
				sh->state = ESsquare;
			} else {
				sh->state = ESnormal;
			}
			break;
		case ESsquare:
			sh->state = ESnormal;
			switch (c) {
				case 'A': ret = UP; break;
				case 'B': ret = DOWN; break;
				case 'C': ret = RIGHT; break;
				case 'D': ret = LEFT; break;
				case '3': sh->state = ESwave; break;
				default: break;
			}
			break;
		case ESwave:
			sh->state = ESnormal;
			if (c == '~') {
				ret = DELETE;
			}
			break;
		default: sh->state = ESnormal; break;
	}

	return ret;
}

void _shell_init(struct nr_micro_shell *sh)
{
	sh->cursor_pos = 0;
	sh->rcv_len = 0;
	sh->state = ESnormal;
	sh->line[0] = '\0';

#ifdef NR_SHELL_HISTORY_CMD_SUPPORT
	sh->his_cmd_request_index = 0;
	sh->his_cmd_rear = 0;
	sh->his_cmd_count = 0;
	sh->shadow_line[0] = '\0';
#endif
}

void shell_init(void)
{
	_shell_init(&_default_sh);

#ifdef NR_SHELL_DEBUG
	nr_shell_debug_log_init();
#endif

#ifdef NR_SHELL_SHOW_LOGO
	shell_printf("                                                              \r\n");
	shell_printf(" _   _ ____    __  __ _                  ____  _          _ _ \r\n");
	shell_printf("| \\ | |  _ \\  |  \\/  (_) ___ _ __ ___   / ___|| |__   ___| | |\r\n");
	shell_printf("|  \\| | |_) | | |\\/| | |/ __| '__/ _ \\  \\___ \\| '_ \\ / _ \\ | |\r\n");
	shell_printf("| |\\  |  _ <  | |  | | | (__| | | (_) |  ___) | | | |  __/ | |\r\n");
	shell_printf("|_| \\_|_| \\_\\ |_|  |_|_|\\___|_|  \\___/  |____/|_| |_|\\___|_|_|\r\n");
	shell_printf("                                                              \r\n");
	shell_printf("Welcome to the nr_micro_shell! build time %s %s\r\n", __DATE__, __TIME__);
#endif
	print_prompt(&_default_sh);
}

void shell(char c)
{
	keycode_t ret;
	ret = get_keycode(&_default_sh, c);
	KEY_RECORD(c);
	run_key_func(ret, &_default_sh, c);
}

/******************* (C) COPYRIGHT 2025 Ji Youzhou ****************************/
