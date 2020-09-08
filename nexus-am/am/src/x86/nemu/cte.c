#include <am.h>
#include <x86.h>
#include<stdio.h>
//#include "monitor/monitor.h"
//#include "monitor/expr.h"
//#include "monitor/watchpoint.h"
//#include "nemu.h"
static _Context* (*user_handler)(_Event, _Context*) = NULL;
//void isa_reg_display();
void __am_irq0();
void __am_vecsys();
void __am_vectrap();
void __am_vecnull();
void __am_get_cur_as(_Context *c);
void __am_switch(_Context *c);
_Context* __am_irq_handle(_Context *c) {
  __am_get_cur_as(c);
  _Context *next = c;
  if (user_handler) {
    _Event ev = {0};
	
   // printf("gpr=%x %x %x %x %x %x %x %x\n",c->edi,c->esi, c->ebp, c->esp, c->ebx,c->edx,c->ecx,c->eax);//isa_reg_display();
    //printf("pc=%x\n",c->eip);
    switch (c->irq) {
	  case 0x80:ev.event=_EVENT_SYSCALL;break;
	  case 0x81:ev.event=_EVENT_YIELD;break;
	  case 32:ev.event=_EVENT_IRQ_TIMER;break;
      default: ev.event = _EVENT_ERROR; break;
    }

    next = user_handler(ev, c);
    if (next == NULL) {
      next = c;
    }
  }
  __am_switch(next);
  return next;
}

int _cte_init(_Context*(*handler)(_Event, _Context*)) {
  static GateDesc idt[NR_IRQ];//64*256 bits
  
  // initialize IDT
  for (unsigned int i = 0; i < NR_IRQ; i ++) {
    idt[i] = GATE(STS_TG32, KSEL(SEG_KCODE), __am_vecnull, DPL_KERN);
  }

  // ----------------------- interrupts ----------------------------
  idt[32]   = GATE(STS_IG32, KSEL(SEG_KCODE), __am_irq0,   DPL_KERN);
  // ---------------------- system call ----------------------------
  idt[0x80] = GATE(STS_TG32, KSEL(SEG_KCODE), __am_vecsys, DPL_USER);
  idt[0x81] = GATE(STS_TG32, KSEL(SEG_KCODE), __am_vectrap, DPL_KERN);
 // printf("%u\n",__am_vectrap);
  set_idt(idt, sizeof(idt));

  // register event handler
  user_handler = handler;
  return 0;
}

_Context *_kcontext(_Area stack, void (*entry)(void *), void *arg) {
	//TODO
//	return NULL;
	_Context *new_p=(_Context*)(stack.end-sizeof(_Context));
	new_p->eip=(uintptr_t)entry;
	new_p->eflags=0x2;
	new_p->cs=8;
	return new_p;
}

void _yield() {
  asm volatile("int $0x81");
}

int _intr_read() {
  return 0;
}

void _intr_write(int enable) {
}
