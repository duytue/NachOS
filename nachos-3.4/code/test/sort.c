// A program to allow user to input an one-dimensional array and sort it

#include "syscall.h"

#define N 100

void printIntArr(int a[], int n) {
	int i;
	for (i = 0; i < n; ++i) {
		PrintInt(a[i]);
		PrintChar(' ');
	}
	PrintChar('\n');
}

void bubbleSortIntAsc(int a[], int n) {
	int i, j, tmp;
	
	for (j = n - 1; j > 0; --j)
		for (i = 0; i < j; ++i)
			if (a[i] > a[i + 1]) {
				tmp = a[i];
				a[i] = a[i + 1];
				a[i + 1] = tmp;
			}
}

int main() {
	int i, n, a[N];
	
	PrintString("Note: when you input an invalid string for an integer, we will read as many digit(s) as we can until an invalid character, instead of get rid of the whole string.\n\n");
	
	do {
		PrintString("Please input the array size (<= 100): ");
		n = ReadInt();
	} while (n < 0 || n > 100);
	PrintString("Size got: ");
	PrintInt(n);
	PrintChar('\n');
	
	for (i = 0; i < n; ++i) {
		PrintString("\nPlease input element ");
		PrintInt(i);
		PrintString(": ");
		a[i] = ReadInt();
		PrintString("\tGot: ");
		PrintInt(a[i]);
	}
	PrintString("\n\n");
	
	PrintString("Got array:\n\t");
	printIntArr(a, n);
	
	bubbleSortIntAsc(a, n);
	PrintString("Sorted ascendingly:\n\t");
	printIntArr(a, n);
	
	Halt();
	return 0;
}
