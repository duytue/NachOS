#include "syscall.h"

#define FNAME_LEN	200
#define STR_LEN		200

int main() {
	char buf[FNAME_LEN], content[STR_LEN];
	int fileFlag;
	
	PrintString("Please input a file name: ");
	ReadString(buf, FNAME_LEN, '\n');
	
	PrintString("Openning file with name \"");
	PrintString(buf);
	PrintString("\"...\n");
	
	fileFlag = OpenFile(buf, 1);
	if (fileFlag == -1)
		PrintString("FAILED!\n");
	else {
		PrintString("SUCCESSFULLY!\n");
		
		PrintString("\t* File content:\n");
		ReadFile(content, STR_LEN, fileFlag);
		PrintString(content);
		
		CloseFile(fileFlag);
	}

	Halt();
	return 0;
}
