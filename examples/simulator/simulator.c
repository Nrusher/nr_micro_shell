#include <stdlib.h>
#include <stdio.h>
#include <curses.h>
#include <unistd.h>
#include <stdint.h>
#include "nr_micro_shell.h"

int main(int argc, char *argv[])
{
	int c;
	int i = 0;

	system("stty -echo");
	system("stty -icanon");
	shell_init();
#ifdef NR_SHELL_DEBUG
#if defined (NR_SHELL_CMD_RD) || defined (NR_SHELL_CMD_WR)
	/* To test wr/rd cmd, malloc 4KB mem*/
	uint8_t *buf = malloc(4096);
	if (buf == NULL) {
		shell_printf( "malloc failed\r\n");
		return -1;
	}

	for (i = 0; i < 4096; i++) {
        buf[i] = (unsigned char)(i % 256);
    }
	shell_printf("malloc 4KB mem, address: %p\r\n", buf);
#endif
#endif
	while (1) {
		c = getchar();
		shell(c);
		if (c == 'q') {
			i++;
		} else {
			i = 0;
		}

		if (i > 3) {
			break;
		}
	}
	system("stty echo");
	system("stty icanon");
	return 0;
}