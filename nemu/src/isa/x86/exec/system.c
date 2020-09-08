#include "cpu/exec.h"
//#include<stdio.h>
void raise_intr(uint32_t NO, vaddr_t ret_addr);
make_EHelper(lidt) {
  //TODO();
  rtl_li(&s0,id_dest->addr);
  //printf("%u %u\n",cpu.idtr.limit,id_src->val&0xfff);
  //if (id_src->width==2) cpu.idtr.base=((id_src->val>>16)&(0x00FFFFFF));
  //else cpu.idtr.base=(id_src->val>>16);
  cpu.idtr.limit=vaddr_read(s0,2);
  cpu.idtr.base=vaddr_read(s0+2,4);
  //printf("%u\n",cpu.idtr.limit);
  print_asm_template1(lidt);
}

make_EHelper(mov_r2cr) {
  //TODO();
  if (id_dest->reg==0) cpu.cr0.val=id_src->val;
  else cpu.cr3.val=id_src->val;
  print_asm("movl %%%s,%%cr%d", reg_name(id_src->reg, 4), id_dest->reg);
}

make_EHelper(mov_cr2r) {
  //TODO();
  if (id_src->reg==0) cpu.gpr[id_dest->reg]._32=cpu.cr0.val;
  else cpu.gpr[id_dest->reg]._32=cpu.cr3.val;
  print_asm("movl %%cr%d,%%%s", id_src->reg, reg_name(id_dest->reg, 4));

  difftest_skip_ref();
}

make_EHelper(int) {
//  TODO();
  switch(decinfo.opcode){
    case 0xcc : raise_intr(0x3,decinfo.seq_pc);  break;
    case 0xcd : raise_intr(id_dest->val, decinfo.seq_pc); break;
    case 0xce : raise_intr(0x4, decinfo.seq_pc); break;
//    default : raise_intr((uint8_t)id_dest->val, decoding.seq_eip);
    }
  /*if (decinfo.opcode==0xcc)
  {
	  raise_intr(3,cpu.pc);
  }
  else
  {
	  //printf("%u\n",id_dest->val);
	  raise_intr(id_dest->val,cpu.pc);
  }*/
  print_asm("int %s", id_dest->str);

  difftest_skip_dut(1, 2);
}

make_EHelper(iret) {
  //TODO();
  rtl_pop(&s0);rtl_j(s0);
  rtl_pop(&cpu.cs);
  rtl_pop(&cpu.eflags.val);
  
  print_asm("iret");
}

uint32_t pio_read_l(ioaddr_t);
uint32_t pio_read_w(ioaddr_t);
uint32_t pio_read_b(ioaddr_t);
void pio_write_l(ioaddr_t, uint32_t);
void pio_write_w(ioaddr_t, uint32_t);
void pio_write_b(ioaddr_t, uint32_t);

make_EHelper(in) {
//  TODO();
  if (id_dest->width==4) s0=pio_read_l(id_src->val);
  else if (id_dest->width==2) s0=pio_read_w(id_src->val);
  else s0=pio_read_b(id_src->val);
  operand_write(id_dest,&s0);
  print_asm_template2(in);
}

make_EHelper(out) {
//  TODO();
	//printf("???\n");
	if (id_dest->width==4)  pio_write_l(id_dest->val,id_src->val);
	else if (id_dest->width==2)  pio_write_w(id_dest->val,id_src->val);
	else if (id_dest->width==1) pio_write_b(id_dest->val,id_src->val);
	else assert(0);
	//printf("444\n");
	print_asm_template2(out);
	//print_asm("777");
}
