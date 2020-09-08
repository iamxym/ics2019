#include <unistd.h>
#include <sys/stat.h>
#include <setjmp.h>
#include <sys/time.h>
#include <assert.h>
#include <time.h>
#include "syscall.h"

// helper macros
#define _concat(x, y) x ## y
#define concat(x, y) _concat(x, y)
#define _args(n, list) concat(_arg, n) list
#define _arg0(a0, ...) a0
#define _arg1(a0, a1, ...) a1
#define _arg2(a0, a1, a2, ...) a2
#define _arg3(a0, a1, a2, a3, ...) a3
#define _arg4(a0, a1, a2, a3, a4, ...) a4
#define _arg5(a0, a1, a2, a3, a4, a5, ...) a5

// extract an arguments from the macro array
#define SYSCALL  _args(0, ARGS_ARRAY)
#define GPR1 _args(1, ARGS_ARRAY)
#define GPR2 _args(2, ARGS_ARRAY)
#define GPR3 _args(3, ARGS_ARRAY)
#define GPR4 _args(4, ARGS_ARRAY)
#define GPRx _args(5, ARGS_ARRAY)

// ISA-depedent definitions
#if defined(__ISA_X86__)
# define ARGS_ARRAY ("int $0x80", "eax", "ebx", "ecx", "edx", "eax")
#elif defined(__ISA_MIPS32__)
# define ARGS_ARRAY ("syscall", "v0", "a0", "a1", "a2", "v0")
#elif defined(__ISA_RISCV32__)
# define ARGS_ARRAY ("ecall", "a7", "a0", "a1", "a2", "a0")
#elif defined(__ISA_AM_NATIVE__)
# define ARGS_ARRAY ("call *0x100000", "rax", "rdi", "rsi", "rdx", "rax")
#else
#error syscall is not supported
#endif
//#include<stdio.h>
extern char _end;

//static intptr_t init_brk=NULL;
intptr_t _syscall_(intptr_t type, intptr_t a0, intptr_t a1, intptr_t a2) {
	//printf("%d\n",type);
  //if (type!=SYS_write) assert(0);
  register intptr_t _gpr1 asm (GPR1) = type;
  register intptr_t _gpr2 asm (GPR2) = a0;
  register intptr_t _gpr3 asm (GPR3) = a1;
  register intptr_t _gpr4 asm (GPR4) = a2;
  register intptr_t ret asm (GPRx);
  asm volatile (SYSCALL : "=r" (ret) : "r"(_gpr1), "r"(_gpr2), "r"(_gpr3), "r"(_gpr4));
  return ret;
}

void _exit(int status) {
  //printf("233\n");
  _syscall_(SYS_exit, status, 0, 0);
  while (1);
}

int _open(const char *path, int flags, mode_t mode) {
  //_exit(SYS_open);

  int ret=_syscall_(SYS_open,(intptr_t)path,0,0);
  return ret;
}

int _write(int fd, void *buf, size_t count) {
  //_exit(SYS_write);
	/*if (fd==1||fd==2)
	{
		char *poi=(char*)buf;
		int i;
		for (i=0;i<count;++i,++poi) putc(*poi);
		return count;
	}*/
	//return _syscall_(SYS_write,fd,buf,count);
	int ret=(int)_syscall_((intptr_t)SYS_write,(intptr_t)fd,(intptr_t)buf,(intptr_t)count);
	
	//_exit(0);
	return ret;
}

/*static intptr_t pbrk=NULL;	
void *_sbrk(intptr_t increment) {
  intptr_t obrk=pbrk;
  intptr_t nbrk=obrk+increment;
  if(pbrk==NULL) pbrk=(uintptr_t)&_end;
  int ret1=_syscall_(SYS_brk,obrk,increment,0);
  if(ret1==0) 
  {
    pbrk=nbrk;
    return (void*)obrk;
  }
  else
  return (void *)-1;
}*/
//static uintptr_t init_brk=NULL;
static uintptr_t init_brk=(uintptr_t)&_end;
void *_sbrk(intptr_t increment) {
  //return (void *)-1;
	//if (init_brk==NULL) init_brk=(uintptr_t)&_end;
	intptr_t pre_brk=init_brk,now_brk=pre_brk+increment;
	//if (init_brk==NULL) init_brk=(uintptr_t)&_end;
	if (_syscall_(SYS_brk,pre_brk,increment,0)==0)
	{
		init_brk=now_brk;
		return (void*)pre_brk;
	}
	else return (void*)-1;
}

int _read(int fd, void *buf, size_t count) {
  //_exit(SYS_read);
  int ret=_syscall_(SYS_read,(intptr_t)fd,(intptr_t)buf,(intptr_t)count);
  return ret;
}

int _close(int fd) {
  //_exit(SYS_close);
  int ret=_syscall_(SYS_close,(intptr_t)fd,0,0);
  return ret;
}

off_t _lseek(int fd, off_t offset, int whence) {
  //_exit(SYS_lseek);
  off_t ret=_syscall_(SYS_lseek,(intptr_t)fd,(intptr_t)offset,(intptr_t)whence);
  return ret;
}

int _execve(const char *fname, char * const argv[], char *const envp[]) {
  //_exit(SYS_execve);
  int ret=_syscall_(SYS_execve,(intptr_t)fname,(intptr_t)argv,(intptr_t)envp);
  return ret;
}

// The code below is not used by Nanos-lite.
// But to pass linking, they are defined as dummy functions

int _fstat(int fd, struct stat *buf) {
  return 0;
}

int _kill(int pid, int sig) {
  _exit(-SYS_kill);
  return -1;
}

pid_t _getpid() {
  _exit(-SYS_getpid);
  return 1;
}

pid_t _fork() {
  assert(0);
  return -1;
  //syscall_(SYS_fork, gamen, 0, 0);   return 0;
}

int _link(const char *d, const char *n) {
  assert(0);
  return -1;
}

int _unlink(const char *n) {
  assert(0);
  return -1;
}

pid_t _wait(int *status) {
  assert(0);
  return -1;
}

clock_t _times(void *buf) {
  assert(0);
  return 0;
}

int _gettimeofday(struct timeval *tv) {
  assert(0);
  tv->tv_sec = 0;
  tv->tv_usec = 0;
  return 0;
}
