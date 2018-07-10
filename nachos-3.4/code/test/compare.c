// A program to test the system call to compare integers

#include "syscall.h"

int main()
{
	int comp;
	char c;
	
	// The new version of Compare system call will no longer print a result message
	// Compare will be used as a helper to other task
	comp = Compare(12, 11);
	comp = Compare(1000, 1000);
	comp = Compare(-4, 0);
	
	Halt(); // To escape from NachOS in Linux terminal
	return 0;
}
