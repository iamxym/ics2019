#include "common.h"
#include "syscall.h"
#include "proc.h"
//#include<stdio.h>
extern char _end;
 typedef size_t off_t;
 typedef size_t ssize_t;

int fs_open(const char *pathname);
size_t fs_read(int fd, void *buf, size_t len);
int fs_close(int fd);
size_t fs_lseek(int fd, size_t offset, int whence);
size_t fs_write(int fd,const void *buf,size_t len);
int mm_brk(uintptr_t brk, intptr_t increment);
void naive_uload(PCB *pcb, const char *filename);
_Context* schedule(_Context *prev) ;
//int _execve(const char *fname, char * const argv[], char *const envp[]);
_Context* do_syscall(_Context *c) {
  uintptr_t a[4];
  a[0] = c->GPR1;
  a[1] = c->GPR2;
  a[2]= c->GPR3;
  a[3]= c->GPR4;
  //printf("%d %d %d %d\n",c->GPR1,c->GPR2,c->GPR3,c->GPR4);
  //printf("ooo=%d\n",a[0]==SYS_yield);//assert(0);
  //Log("This is yes!");
  switch (a[0]) {
	case SYS_yield:Log("!!!");c->GPRx=0;return schedule(c);break;
	case SYS_exit:c->GPRx=0;_halt(a[1]);/*naive_uload(NULL,"/bin/init");c->GPRx=0;*/break;
	case SYS_write:
	{
		//assert(0);
		//Log("%d %d %d",a[1],a[2],a[3]);
		/*int fd=(int)c->GPR2;
		char* buf=(char *)c->GPR3;
		size_t len=(size_t)c->GPR4;//if (len!=14) assert(0);
		if (fd!=1&&fd!=2)c->GPRx=fs_write((int)a[1],(const void *)a[2],(size_t)a[3]);// c->GPRx=-1;
		else
		{
			for (int i=0;i<len;++i,++buf) _putc(*buf);
			c->GPRx=len;
		}*/
		c->GPRx=fs_write((int)a[1],(const void *)a[2],(size_t)a[3]);
		break;
	}
	case SYS_brk:
	{
		Log("brk syscall");
		//_end=*((char*)c->GPR2);
		mm_brk((uintptr_t)a[1],(intptr_t)a[2]);
		c->GPRx=0;
		break;
	}
	case SYS_open:/*Log("open syscall");*/c->GPRx=fs_open((const char *)a[1]);break;
	case SYS_read:/*Log("read syscall");*/c->GPRx=fs_read((int)a[1],(void *)a[2],(size_t)a[3]);break;
	case SYS_close:/*Log("close syscall");*/c->GPRx=fs_close((int)a[1]);break;
	case SYS_lseek:/*Log("lseek syscall");*/c->GPRx=fs_lseek((int)a[1],(size_t)a[2],(int)a[3]);break;
	case SYS_execve:/*Log("execve syscall");*/naive_uload(NULL,(char *)a[1]);c->GPRx=0;break;
    default: panic("Unhandled syscall ID = %d", a[0]);
  }

  return NULL;
}
