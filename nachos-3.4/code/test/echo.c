#include "syscall.h"

int main() {
	const int n = 1000;
	char buff[n];
	
	int cin = 0, cout = 1;
	int charr, charw;
	
	PrintString("WELCOME TO NACHOS CONSOLE I/O\nFeel free to input anything...\nIf you get bored or stuck, press Ctrl + C or Ctrl + Z to exit!\n\n");
	
	while(1) {
		charr = ReadFile(buff, n, cin);
		WriteFile("\n", 1, cin);
		PrintString("\t* Echo back:\n");
		charw =	WriteFile(buff, charr, cout);
		WriteFile("\n", 1, cout);
		PrintString("Char(s) read: ");
		PrintInt(charr);
		PrintString("\nChar(s) written: ");
		PrintInt(charw);
		PrintString("\n------------\n");
	}
	
	Halt();
	return 0;
}
