#include <stdio.h>
#include "nr_micro_shell.h"

int main(void)
{
    char c;
    shell_init();
    while(1)
    {
        c = getchar();
        shell(c);
    }
}