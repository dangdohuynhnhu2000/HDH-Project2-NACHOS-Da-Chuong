/* sort.c 
 *    Test program to sort a large number of integers.
 *
 *    Intention is to stress virtual memory system.
 *
 *    Ideally, we could read the unsorted array off of the file system,
 *	and store the result back to the file system!
 */

#include "syscall.h"

int A[100];	/* size of physical memory; with code, we'll run out of space!*/

int
main()
{
    int i, j, n = 0, tmp, choice = 0;

    /* input n */
    while (n <= 0 || n > 100)
    {
	PrintString("Nhap so nguyen n <= 100: ");
	n = ReadInt();
    }

    /* initialize the array, in reverse sorted order */
    for (i = 0; i < n; i++)		
    {   
	PrintString("\nA[");
	PrintInt(i);
	PrintString("] = ");
	A[i] = ReadInt();
    }

    while (choice != 1 && choice != 2)
    {
	PrintString("\nSap xep mang theo:\n1.Tang dan\n2.Giam dan\nChon ");
	choice = ReadInt();
    }

    if (choice == 1)
	/* ascending order */
	for (i = 0; i < n - 1; i++)
            for (j = 0; j < (n - i - 1); j++)
		if (A[j] > A[j + 1]) {	/* out of order -> need to swap ! */
		    tmp = A[j];
		    A[j] = A[j + 1];
		    A[j + 1] = tmp;
		}
    if (choice == 2)
	/* descending order */
	for (i = 0; i < n - 1; i++)
            for (j = 0; j < (n - i - 1); j++)
		if (A[j] < A[j + 1]) {	/* out of order -> need to swap ! */
		    tmp = A[j];
		    A[j] = A[j + 1];
		    A[j + 1] = tmp;
		}

    //array after sort
    PrintString("Mang da sap xep:\n");
    for (i = 0; i < n; i++)		
    {   
	PrintInt(A[i]);
	PrintChar('\n');
    }
    Halt();		/* and then we're done -- should be 0! */
}
