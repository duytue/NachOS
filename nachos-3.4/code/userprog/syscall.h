/* syscalls.h 
 * 	Nachos system call interface.  These are Nachos kernel operations
 * 	that can be invoked from user programs, by trapping to the kernel
 *	via the "syscall" instruction.
 *
 *	This file is included by user programs and by the Nachos kernel. 
 *
 * Copyright (c) 1992-1993 The Regents of the University of California.
 * All rights reserved.  See copyright.h for copyright notice and limitation 
 * of liability and disclaimer of warranty provisions.
 */

#ifndef SYSCALLS_H
#define SYSCALLS_H

#include "copyright.h"

/* system call codes -- used by the stubs to tell the kernel which system call
 * is being asked for
 */
#define SC_Halt		0
#define SC_Exit		1
#define SC_Exec		2
#define SC_Join		3
#define SC_Fork		9
#define SC_Yield	10
#define SC_Compare	11
#define SC_ReadChar	12
#define SC_PrintChar	13
#define SC_ReadString	14
#define SC_PrintString	15
#define SC_ReadInt	16
#define SC_PrintInt	17
#define SC_CreateFile	18
#define SC_OpenFile		19
#define SC_CloseFile	20
#define SC_ReadFile		21
#define SC_WriteFile	22
#define SC_SeekFile		23

#ifndef IN_ASM

/* The system call interface.  These are the operations the Nachos
 * kernel needs to support, to be able to run user programs.
 *
 * Each of these is invoked by a user program by simply calling the 
 * procedure; an assembly language stub stuffs the system call code
 * into a register, and traps to the kernel.  The kernel procedures
 * are then invoked in the Nachos kernel, after appropriate error checking, 
 * from the system call entry point in exception.cc.
 */

/* Stop Nachos, and print out performance stats */
void Halt();		
 

/* Address space control operations: Exit, Exec, and Join */

/* This user program is done (status = 0 means exited normally). */
void Exit(int status);	

/* A unique identifier for an executing user program (address space) */
typedef int SpaceId;	
 
/* Run the executable, stored in the Nachos file "name", and return the 
 * address space identifier
 */
SpaceId Exec(char *name);
 
/* Only return once the the user program "id" has finished.  
 * Return the exit status.
 */
int Join(SpaceId id); 	

#define ConsoleInput	0  
#define ConsoleOutput	1  
 
/* User-level thread operations: Fork and Yield.  To allow multiple
 * threads to run within a user program. 
 */

/* Fork a thread to run a procedure ("func") in the *same* address space 
 * as the current thread.
 */
void Fork(void (*func)());

/* Yield the CPU to another runnable thread, whether in this address space 
 * or not. 
 */
void Yield();

/* 1: a > b
 * 0: a = b
 * -1: a < b
 */
int Compare(int a, int b);

char ReadChar();
void PrintChar(char c);

// Add delim to ReadString to be more general
// delim denotes string delimiter, e.g. '\n'
void ReadString(char buffer[], int length, char delim);
void PrintString(char buffer[]);

int ReadInt();
void PrintInt(int num);

typedef int OpenFileID;

int CreateFile(char* fileName);
OpenFileID OpenFile(char* filename, int type);
int CloseFile(OpenFileID id);

int ReadFile(char* buffer, int charcount, OpenFileID id);
int WriteFile(char* buffer, int charcount, OpenFileID id);

int SeekFile(int pos, OpenFileID id);

#endif /* IN_ASM */

#endif /* SYSCALL_H */
