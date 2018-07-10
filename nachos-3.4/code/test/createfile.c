// A program to test system call working with files

#include "syscall.h"

#define FNAME_LEN	200

int main() {	
	
	char buf[FNAME_LEN + 1];
	int fileFlag;
	
	PrintString("Please input a file name (max ");
	PrintInt(FNAME_LEN);
	PrintString(" characters): ");
	ReadString(buf, FNAME_LEN, '\n');
	
	PrintString("Creating file with name \"");
	PrintString(buf);
	PrintString("\"...\n");
	
	fileFlag = CreateFile(buf);
	if (fileFlag != 0)
		PrintString("FAILED!\n");
	else
		PrintString("SUCCESSFULLY!\n");
	
	Halt();
	return 0;
}
