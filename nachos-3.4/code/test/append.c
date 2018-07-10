#include "syscall.h"

#define FNAME_LEN	200
#define STR_LEN		200

int main() {
	char source1[FNAME_LEN], source2[FNAME_LEN], dest[FNAME_LEN], content[STR_LEN];
	int src1, src2, dst, n;
	
	PrintString("Please input source file name: ");
	ReadString(source1, FNAME_LEN, '\n');
	
	PrintString("Please input another source file name: ");
	ReadString(source2, FNAME_LEN, '\n');
	
	PrintString("Openning these 2 files...\n");
	
	src1 = OpenFile(source1, 1);
	src2 = OpenFile(source2, 1);
	if (src1 == -1 || src2 == -1)
		PrintString("FAILED!\n");
	else {
		PrintString("SUCCESSFULLY!\n");
		
		PrintString("Please input destination file name: ");
		ReadString(dest, FNAME_LEN, '\n');
	
		if (CreateFile(dest) == -1)
			PrintString("Cannot create the destination file!\n");
		else {
			dst = OpenFile(dest, 0);
			n = ReadFile(content, STR_LEN, src1);		
			WriteFile(content, n, dst);
			n = ReadFile(content, STR_LEN, src2);		
			WriteFile(content, n, dst);
			PrintString("Content from the 2 sources joint into destination!\n");
			
			CloseFile(dst);
		}
		
		CloseFile(src1);
		CloseFile(src2);
	}

	Halt();
	return 0;
}


						
