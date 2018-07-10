// exception.cc
//	Entry point into the Nachos kernel from user programs.
//	There are two kinds of things that can cause control to
//	transfer back to here from user code:
//
//	syscall -- The user code explicitly requests to call a procedure
//	in the Nachos kernel.  Right now, the only function we support is
//	"Halt".
//
//	exceptions -- The user code does something that the CPU can't handle.
//	For instance, accessing memory that doesn't exist, arithmetic errors,
//	etc.
//
//	Interrupts (which can also cause control to transfer from user
//	code into the Nachos kernel) are handled elsewhere.
//
// For now, this only handles the Halt() system call.
// Everything else core dumps.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "system.h"
#include "syscall.h"

//----------------------------------------------------------------------
// ExceptionHandler
// 	Entry point into the Nachos kernel.  Called when a user program
//	is executing, and either does a syscall, or generates an addressing
//	or arithmetic exception.
//
// 	For system calls, the following is the calling convention:
//
// 	system call code -- r2
//		arg1 -- r4
//		arg2 -- r5
//		arg3 -- r6
//		arg4 -- r7
//
//	The result of the system call, if any, must be put back into r2.
//
// And don't forget to increment the pc before returning. (Or else you'll
// loop making the same system call forever!
//
//	"which" is the kind of exception.  The list of possible exceptions
//	are in machine.h.
//----------------------------------------------------------------------

#define ret 2
#define arg1 4
#define arg2 5
#define arg3 6
#define arg4 7
#define INT_MAX 2147483647
#define INT_MIN -2147483648
#define INT_LEN 10 // typical maximum length for a string representing an int, including sign

void SysCall_Halt() {
	DEBUG('a', "Shutdown, initiated by user program!\n");
	interrupt->Halt();
}

void SysCall_Compare() {
	int op1, op2, comp;
	op1 = machine->ReadRegister(arg1);
	op2 = machine->ReadRegister(arg2);
	if (op1 < op2)
		comp = -1;
	else if (op1 == op2)
		comp = 0;
	else
		comp = 1;
	machine->WriteRegister(ret,  comp);
}

void SysCall_ReadChar() {
	char c = 0;
	int nbytes = 1, byteres = synchCons->Read(&c, nbytes);
	if (byteres < nbytes)
		DEBUG('a', "Unsuccessful console I/O, %d byte(s) read instead of %d\n", byteres, nbytes);
	machine->WriteRegister(ret, c);
}

void SysCall_PrintChar() {
	char c = machine->ReadRegister(arg1);
	int nbytes = 1, byteres = synchCons->Write(&c, nbytes);
	if (byteres < nbytes)
		DEBUG('a', "Unsuccessful console I/O, %d byte(s) written instead of %d\n", byteres, nbytes);
	machine->WriteRegister(ret, 0);
}

void SysCall_ReadString() {
	int virBuf = machine->ReadRegister(arg1);
	int len = machine->ReadRegister(arg2);
	char delim = machine->ReadRegister(arg3);
	char *buf = NULL, c;
	int byteres, i;

	buf = new char[len + 1];
	if (buf == NULL) {
		DEBUG('a', "Unsuccessful memory allocation for an array of %d byte(s)\n", len + 1);
		machine->WriteRegister(ret, 0);
		return;
	}

	for (i = 0; i < len; ++i) {
		synchCons->Read(&c, 1);
		if (c == delim)
			c = 0;
		buf[i] = c;
		if (buf[i] == 0)
			break;
	}
	if (i == len)
		buf[i] = 0;
	++i;
	byteres = machine->System2User(virBuf, i, buf);
	if (byteres < i)
		DEBUG('a', "Unsuccessful memory access, %d byte(s) written instead of %d\n", byteres, i);

	machine->WriteRegister(ret, 0);
	delete[] buf;
}

void SysCall_PrintString() {
	int virBuf = machine->ReadRegister(arg1);
	int limit = machine->FindStrLim(virBuf), len = limit - 1;
	char *str = machine->User2System(virBuf, limit);
	int byteres;

	if (str == NULL) {
		DEBUG('a', "Unsuccessful read from user memory at location %d\n", virBuf);
		machine->WriteRegister(ret, 0);
		return;
	}

	byteres = synchCons->Write(str, len);
	if (byteres < len)
		DEBUG('a', "Unsuccessful console I/O, %d byte(s) written instead of %d\n", byteres, len);

	machine->WriteRegister(ret, 0);
	delete[] str;
}

bool isDigit(char c) {
	return (c >= '0' && c <= '9');
}

bool isSpace(char c) {
	return (c == ' ' || c == '\t' || c == '\v' || c == '\f' || c == 0);
	// Synch Console will return 0 when reading 'enter'
}

void SysCall_ReadInt() {
	long long number = 0, lim = INT_MAX;
	int res;
	short sign = 1;
	char c;
	
	synchCons->Read(&c, 1); // may get stuck here, reason unknown, and it's not our fault
	// bullshit Synch Console!
	
	// Ignore trailing empty space(s)
	while (isSpace(c))
		synchCons->Read(&c, 1);
	
	if (c == '-') {
		sign = -1;
		lim = -INT_MIN;
	}
	if (c == '-' || c == '+')
		synchCons->Read(&c, 1);
	
	while (isDigit(c)) {
		// C compiler accepts overflow: read value may differ from the long input string
		// but here we will read as many digits as we can, then ignore the redundant digits
		number = number * 10 + (c - '0');
		if (number > lim) {
			number /= 10;
			break;
		}
		synchCons->Read(&c, 1);
	}
	
	// c is now hold a char that may be needed for the next input
	// but no way to push c back to input stream/buffer with Synch Console class

	// To be simple, let's ignore all (invalid) chars until an empty space
	while (!isSpace(c))
		synchCons->Read(&c, 1);
	
	number *= sign;
	res = number; // will raise type-cast warning

	DEBUG('a', "Integer read: %d\n", res);
	machine->WriteRegister(ret, res);
}

void SysCall_PrintInt() {
	int number = machine->ReadRegister(arg1), i = 0;
	char buff[INT_LEN];
	char neg = '-';
	
	if (number < 0) {
		synchCons->Write(&neg, 1);
		number = -number;
	}
		
	do {
		buff[i++] = (number % 10) + '0';
		number /= 10;
	} while (number > 0);
	
	for (i -= 1; i >= 0; --i)
		synchCons->Write(&buff[i], 1);

	machine->WriteRegister(ret, 0);
}

void SysCall_CreateFile() {
	int virBuf = machine->ReadRegister(arg1);
	int limit = machine->FindStrLim(virBuf);
	char *str = machine->User2System(virBuf, limit);

	if (str == NULL) {
		DEBUG('a', "Unsuccessful read from user memory at location %d\n", virBuf);
		machine->WriteRegister(ret, 0);
		return;
	}

	if (fileSystem->Create(str, 0) == false) {
		DEBUG('f', "Unsuccessful file creation of name %s\n", str);
		machine->WriteRegister(ret, -1);
	} else {
		DEBUG('f', "Successful file creation of name %s\n", str);
		machine->WriteRegister(ret, 0);
	}

	delete[] str;
}

void SysCall_OpenFile() {
	int virBuf = machine->ReadRegister(arg1);
	int limit = machine->FindStrLim(virBuf);
	int type = machine->ReadRegister(arg2);

	// Check for #descriptors lim
	if (fileSystem->fcur >= fDescMax) {
		DEBUG('f', "Not enough rooms for file descriptors\n");
		machine->WriteRegister(ret, -1);
		return;
	}

	char *str = machine->User2System(virBuf, limit);

	if (strcmp(str, NCIN) == 0) {
		printf("Accessing NachOS Console Input\n");
		machine->WriteRegister(ret, 0);
		return;
	}

	if (strcmp(str, NCOUT) == 0) {
		printf("Accessing NachOS Console Output\n");
		machine->WriteRegister(ret, 1);
		return;
	}

	
	if (fileSystem->Open(str, type)) {
		DEBUG('f', "Open file of name %s successfully\n", str);
		machine->WriteRegister(ret, fileSystem->fcur);
	} else {
		DEBUG('f', "Open file of name %s UNsuccessfully\n", str);
		machine->WriteRegister(ret, -1);
	}

	delete[] str;
}

void SysCall_CloseFile() {
	int idx = machine->ReadRegister(arg1);
	if (idx < 0 || idx >= fDescMax || !fileSystem->openFiles[idx]) {
		machine->WriteRegister(ret, -1);
		return;
	}
	delete fileSystem->openFiles[idx];
	fileSystem->openFiles[idx] = NULL;
	machine->WriteRegister(ret, 0);
}

void SysCall_ReadFile() {
	int virBuf = machine->ReadRegister(arg1);
	int charcount = machine->ReadRegister(arg2);
	OpenFileID fileid = machine->ReadRegister(arg3);
	
	if (fileid < 0 || fileid >= fDescMax || !fileSystem->openFiles[fileid]) {
		machine->WriteRegister(ret, -1);
		return;
	}
	
	char* buf = new char[charcount];
	int byteres;
	
	if (fileSystem->openFiles[fileid]->type == -1) { // NachOS Console Input
		char c;
		int i;
		for (i = 0; i < charcount - 1; ++i) {
			synchCons->Read(&c, 1);
			if (c == '\n')
				c = 0;
			buf[i] = c;
			if (buf[i] == 0)
				break;
		}
		if (i == charcount - 1)
			buf[i] = 0;
		byteres = i;
		
		machine->System2User(virBuf, byteres, buf);
		machine->WriteRegister(ret, byteres);
		fileSystem->openFiles[fileid]->Write(buf, byteres);
		delete[] buf;
		return;
	}
	
	byteres = fileSystem->openFiles[fileid]->Read(buf, charcount); 
	
	if (byteres > 0) {
		machine->System2User(virBuf, byteres, buf);
		machine->WriteRegister(ret, byteres);
	}
	delete[] buf;
}

void SysCall_WriteFile() {
	int virBuf = machine->ReadRegister(arg1);
	int charcount = machine->ReadRegister(arg2);
	OpenFileID fileid = machine->ReadRegister(arg3);
	
	if (fileid < 0 || fileid >= fDescMax || !fileSystem->openFiles[fileid]) {
		machine->WriteRegister(ret, -1);
		return;
	}
	
	if (fileSystem->openFiles[fileid]->type == 1) { // read-only
		machine->WriteRegister(ret, -1);
		return;
	}
	
	char* buf = machine->User2System(virBuf, charcount);
	int byteres;
	
	if (fileSystem->openFiles[fileid]->type == -2) { // NachOS Console Output
		byteres = synchCons->Write(buf, charcount);
		machine->System2User(virBuf, byteres, buf);
		machine->WriteRegister(ret, byteres);
		fileSystem->openFiles[fileid]->Write(buf, byteres);
		delete[] buf;
		return;
	}
	
	byteres = fileSystem->openFiles[fileid]->Write(buf, charcount); 
	
	if (byteres > 0) {
		machine->System2User(virBuf, byteres, buf);
		machine->WriteRegister(ret, byteres);
	}
	delete[] buf;
}

void SysCall_SeekFile() {
	int pos = machine->ReadRegister(arg1);
	OpenFileID fileid = machine->ReadRegister(arg2);
	
	if (fileid < 0 || fileid >= fDescMax || !fileSystem->openFiles[fileid]) {
		machine->WriteRegister(ret, -1);
		return;
	}
	
	if (fileSystem->openFiles[fileid]->type == -1 || fileSystem->openFiles[fileid]->type == -2) {
		DEBUG('f', "Trying to modify console I/O... Denied!\n");
		machine->WriteRegister(ret, -1);
		return;
	}
	
	if (pos < 0 || pos > fileSystem->openFiles[fileid]->Length()) {
		machine->WriteRegister(ret, -1);
		return;
	}
	
	if (pos == -1)
		pos = fileSystem->openFiles[fileid]->Length();
	
	fileSystem->openFiles[fileid]->Seek(pos);
	machine->WriteRegister(ret, pos);
}

void ExceptionHandler(ExceptionType which) {
   	int type = machine->ReadRegister(ret);
   	bool valid = true;

	switch(which) {
		case SyscallException:
		switch(type) {
			case SC_Halt:
			SysCall_Halt();
			break;

			case SC_Compare:
			SysCall_Compare();
			break;

			case SC_ReadChar:
			SysCall_ReadChar();
			break;

			case SC_PrintChar:
			SysCall_PrintChar();
			break;

			case SC_ReadString:
			SysCall_ReadString();
			break;

			case SC_PrintString:
			SysCall_PrintString();
			break;

			case SC_ReadInt:
			SysCall_ReadInt();
			break;

			case SC_PrintInt:
			SysCall_PrintInt();
			break;
			
			case SC_CreateFile:
			SysCall_CreateFile();
			break;
			
			case SC_OpenFile:
			SysCall_OpenFile();
			break;

			case SC_CloseFile:
			SysCall_CloseFile();
			break;

			case SC_ReadFile:
			SysCall_ReadFile();
			break;

			case SC_WriteFile:
			SysCall_WriteFile();
			break;

			case SC_SeekFile:
			SysCall_SeekFile();
			break;

			default:
			valid = false;
			break;
		}

		break;

		// case <Another type of exception>

		default:
		valid = false;
		break;
	}

	// Increment PC (even if the call is invalid)
	machine->registers[PrevPCReg] = machine->registers[PCReg];
	machine->registers[PCReg] = machine->registers[NextPCReg];
	machine->registers[NextPCReg] += 4;

	if (!valid) {
		DEBUG('a', "Unexpected user mode exception %d %d\n", which, type);
		ASSERT(FALSE);
    }
}
