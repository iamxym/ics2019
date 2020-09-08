#include "memory.h"
#include "proc.h"
static void *pf = NULL;
extern PCB *current;
void* new_page(size_t nr_page) {
  void *p = pf;
  pf += PGSIZE * nr_page;
  assert(pf < (void *)_heap.end);
  return p;
}

void free_page(void *p) {
  panic("not implement yet");
}


/* The brk() system call handler. */
int mm_brk(uintptr_t brk, intptr_t increment) {
	if (brk+increment>current->max_brk)
	{
		int new_pgnum=((brk+increment-current->max_brk)+0xfff)/PGSIZE;
		for (int i=new_pgnum-1;i>=0;--i)
		{
			void *pa=new_page(1);
			_map(&(current->as),(void*)(current->max_brk),pa,0);
			current->max_brk+=PGSIZE;
		}
	}
	/*uintptr_t now_brk=brk+increment;
	if (current->max_brk>=now_brk) return 0;
	void *pa;
	while (current->max_brk<now_brk)
	{
		pa=new_page(1);
		//if ((uintptr_t)pa&0xfff) panic("implement");
		_map(&(current->as),(void*)(current->max_brk),pa,1);
		current->max_brk+=PGSIZE;
	}*/
 /* uintptr_t nbrk=brk+increment;
  if(current->max_brk<nbrk){
     uintptr_t st=brk;
     int cnt=nbrk-(current->max_brk);
     int pcnt=(cnt-1)/PGSIZE+1;
     for(int i=0;i<pcnt;i++)
     {
       void* pagen=new_page(1);
       _map(&(current->as),(void*)st,pagen,1);
       st+=PGSIZE;
     }
     current->max_brk=brk+increment;
  }*/
  return 0;
}
void init_mm() {
  pf = (void *)PGROUNDUP((uintptr_t)_heap.start);
  Log("free physical pages starting from %p", pf);

  _vme_init(new_page, free_page);
}
