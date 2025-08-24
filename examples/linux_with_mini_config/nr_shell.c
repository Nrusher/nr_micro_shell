#include <stdlib.h>
#include <stdio.h>
#include <curses.h>
#include <unistd.h>
#include <stdint.h>
#include "nr_micro_shell.h"

int main(int argc, char *argv[])
{
	int c;

	system("stty -echo");
	system("stty -icanon");
	shell_init();
	while (1) {
		c = getchar();
		shell(c);
	}
	return 0;
}