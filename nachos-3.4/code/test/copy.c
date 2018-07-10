#include "syscall.h"

#define FNAME_LEN	200
#define STR_LEN		200

int main() {
	char source[FNAME_LEN], dest[FNAME_LEN], content[STR_LEN];
	int src, dst, n;
	
	PrintString("Please input source file name: ");
	ReadString(source, FNAME_LEN, '\n');
	
	PrintString("Openning file with name \"");
	PrintString(source);
	PrintString("\"...\n");
	
	src = OpenFile(source, 1);
	if (src == -1)
		PrintString("FAILED!\n");
	else {
		PrintString("SUCCESSFULLY!\n");
		
		PrintString("Please input destination file name: ");
		ReadString(dest, FNAME_LEN, '\n');
	
		if (CreateFile(dest) == -1)
			PrintString("Cannot create the destination file!\n");
		else {
			dst = OpenFile(dest, 0);
			n = ReadFile(content, STR_LEN, src);
			WriteFile(content, n, dst);
			
			PrintString("File copied!\n");
			
			CloseFile(dst);
		}
		
		CloseFile(src);
	}

	Halt();
	return 0;
}
