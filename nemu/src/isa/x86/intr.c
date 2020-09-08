#include "rtl/rtl.h"
#define IRQ_TIMER 32
void raise_intr(uint32_t NO, vaddr_t ret_addr) {
  /* TODO: Trigger an interrupt/exception with ``NO''.
   * That is, use ``NO'' to index the IDT.
   */
	//bool flag=false;
	//if (NO==IRQ_TIMER) flag=true;
	rtl_push(&cpu.eflags.val);
	rtl_push(&cpu.cs);
	rtl_push(&ret_addr);
	//if (flag)
	  cpu.eflags.IF=0;
	uint32_t start=cpu.idtr.base,len=cpu.idtr.limit;
	if (len<=NO)
	{
		printf("the number is larger than the length of IDT!\n");
		assert(0);
	}
	uint32_t val_l,val_h,p;
	val_l=vaddr_read(start+NO*8,2);
	val_h=vaddr_read(start+NO*8+6,2);
	p=vaddr_read(start+NO*8+5,1)>>7;
	if (!p)
	{
		printf("The gatedesc is not allowed!");
		assert(0);	
	}
	vaddr_t goal=(val_h<<16)+val_l;
	rtl_j(goal);
}
bool isa_query_intr(void) {
	if (cpu.INTR==true&&cpu.eflags.IF==1)
	{
		cpu.INTR=false;
		raise_intr(IRQ_TIMER,cpu.pc);
		return true;
	}
	return false;
}
