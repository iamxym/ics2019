#include "proc.h"

#define MAX_NR_PROC 4

static PCB pcb[MAX_NR_PROC] __attribute__((used)) = {};
static PCB pcb_boot = {};
PCB *current = NULL;
void naive_uload(PCB *pcb, const char *filename); 
void context_kload(PCB *pcb, void *entry);
void context_uload(PCB *pcb, const char *filename);
uint32_t fg_pcb;
void change_gcb(uint32_t id){fg_pcb=id;}
void switch_boot_pcb() {
  current = &pcb_boot;
}

void hello_fun(void *arg) {
  int j = 1;
  while (1) {
    Log("???Hello World from Nanos-lite for the %dth time!", j);
    j ++;
    _yield();
  }
}

void init_proc() {


  
  //context_kload(&pcb[0], (void *)hello_fun);//additional
  switch_boot_pcb();
  fg_pcb=1;
  context_uload(&pcb[0], "/bin/hello");
  context_uload(&pcb[1], "/bin/pal");
  context_uload(&pcb[2], "/bin/pal");
  context_uload(&pcb[3], "/bin/pal");
  Log("Initializing processes...");

  // load program here
  //naive_uload(NULL,"/bin/events");
  //Log("4396");
}
uint32_t now_id=0;
_Context* schedule(_Context *prev) {
  //TODO
//  return NULL;
	//return prev;
	//Log("nmsl!");
	current->cp=prev;
	if (now_id!=fg_pcb) now_id=fg_pcb,current=&pcb[fg_pcb];
	else now_id=0,current=&pcb[0];
	//current = (current == &pcb[0] ? &pcb[fg_pcb] : &pcb[0]);
	return current->cp;
}
