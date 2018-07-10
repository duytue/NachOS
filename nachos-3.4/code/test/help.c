// A program printing this project's information on the console

#include "syscall.h"

int main() {
	PrintString("A lab project of CS333 - Operating System course\n");
	PrintString("\tby\n");
	PrintString("Tran Van Duy Tue - 1551044\n");
	PrintString("Lam Le Thanh The - 1551034\n");
	PrintString("Class 15CTT - APCS - HMCUS\n");

	Halt();
	return 0;
}
