/**************************************************************
 *
 * userprog/ksyscall.h
 *
 * Kernel interface for systemcalls 
 *
 * by Marcus Voelp  (c) Universitaet Karlsruhe
 *
 **************************************************************/

#ifndef __USERPROG_KSYSCALL_H__ 
#define __USERPROG_KSYSCALL_H__ 

#include "kernel.h"

#include "synchconsole.h"
#include "openfile.h"

void SysHalt()
{
  kernel->interrupt->Halt();
}

int SysAdd(int op1, int op2)
{
  return op1 + op2;
}

#ifdef FILESYS_STUB
int SysCreate(char *filename)
{
	// return value
	// 1: success
	// 0: failed
	return kernel->interrupt->CreateFile(filename);
}
#else 
int SysCreate(char *filename,int initialSize)
{
	// return value
	// 1: success
	// 0: failed
	return kernel->interrupt->CreateFile(filename,initialSize);
}
#endif
OpenFileId SysOpen(char *name){
    return kernel->interrupt->Open(name);
}

int SysWrite(char *msg, int _size, OpenFileId id){
    return kernel->interrupt->WriteFile(msg, _size, id);
}

int SysRead(char *msg, int _size, OpenFileId id){
    return kernel->interrupt->ReadFile(msg, _size, id);
}

int SysClose(int id){ 
    return kernel->interrupt->Close(id); 
}


#endif /* ! __USERPROG_KSYSCALL_H__ */
