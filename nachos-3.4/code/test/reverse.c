#include "syscall.h"

#define FNAME_LEN	200
#define STR_LEN		200

int main() {
	char source[FNAME_LEN], dest[FNAME_LEN], content[STR_LEN], c;
	int src, dst, n, i, j;
	
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
			
			// Reverse the content
			j = n - 2; // (n-1)th element is the EOF ???, if we swap it to the begining it add 1 extra enter
			for (i = 0; i < j; ++i) {
				c = content[i];
				content[i] = content[j];
				content[j] = c;
				--j;
			}	
			
			WriteFile(content, n, dst);
			PrintString("Content from source reversed and written into destination!\n");
			
			CloseFile(dst);
		}
		
		CloseFile(src);
	}

	Halt();
	return 0;
}


						
