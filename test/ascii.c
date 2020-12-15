#include "syscall.h"

int main()
{
    int i;
    for (i = 32; i < 127; i++)
    {
	PrintInt(i);
	PrintChar('\t');
	PrintChar(i);
	PrintChar('\n');
    }
    Halt();
}
