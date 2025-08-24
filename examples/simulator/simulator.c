#include <stdlib.h>
#include <stdio.h>
#include <curses.h>
#include <unistd.h>
#include <stdint.h>
#include "nr_micro_shell.h"

#ifdef NR_DEBUG_BIN
#include <gcov.h>
#endif

int main(int argc, char *argv[])
{
	int c;
#ifdef NR_DEBUG_BIN
	int i = 0;
#endif

	system("stty -echo");
	system("stty -icanon");
	shell_init();
	while (1) {
		c = getchar();
		shell(c);
#ifdef NR_DEBUG_BIN
	 	// __gcov_dump();
		if (c == 'q') {
			i++;
		} else {
			i = 0;
		}

		if (i > 3) {
			break;
		}
#endif
	}
	system("stty echo");
	system("stty icanon");
	return 0;
}