// A program to print the (extended) ASCII table

#include "syscall.h"

int main() {	
	int i;
	for (i = 0; i < 256; ++i) {
		PrintInt(i);
		PrintChar(' ');
		PrintChar(i);
		PrintChar('\n');
	}
	
	/*
	char c;
	c = ReadChar();
	PrintChar(c);
	*/
	
	Halt();
	return 0;
}
