#include "cpu/exec.h"

make_EHelper(mov) {
  operand_write(id_dest, &id_src->val);
  print_asm_template2(mov);
}

make_EHelper(push)
{
	rtl_push(&id_dest->val);
	print_asm_template1(push);
}

make_EHelper(pop) {
	rtl_pop(&s0);
	operand_write(id_dest,&s0);
	print_asm_template1(pop);
}

make_EHelper(pusha) {
  //TODO();
    rtl_mv(&s0,&cpu.esp);
	rtl_push(&cpu.eax);
	rtl_push(&cpu.ecx);
	rtl_push(&cpu.edx);
	rtl_push(&cpu.ebx);
	rtl_push(&s0);
	rtl_push(&cpu.ebp);
	rtl_push(&cpu.esi);
	rtl_push(&cpu.edi);
	print_asm("pusha");
}

make_EHelper(popa) {
  //TODO();
  rtl_pop(&cpu.edi);
  rtl_pop(&cpu.esi);
  rtl_pop(&cpu.ebp);
  rtl_pop(&s0);
  rtl_pop(&cpu.ebx);
  rtl_pop(&cpu.edx);
  rtl_pop(&cpu.ecx);
  rtl_pop(&cpu.eax);
  //rtl_mv(&cpu.esp,&s0);
  print_asm("popa");
}
make_EHelper(movsb)
{
	rtl_lm(&s0,&cpu.esi,1);
	rtl_sm(&cpu.edi,&s0,1);
	++cpu.esi;
	++cpu.edi;
	print_asm("movsb");
}
make_EHelper(movswd)
{
	if (id_dest->width!=2&&id_dest->width!=4) assert(0);
	rtl_lm(&s0,&cpu.esi,id_dest->width);
	rtl_sm(&cpu.edi,&s0,id_dest->width);
	cpu.esi+=id_dest->width;
	cpu.edi+=id_dest->width;
	print_asm("movswd");
}
make_EHelper(leave) {
//  TODO();
  rtl_mv(&cpu.esp,&cpu.ebp);
  rtl_pop(&cpu.ebp);
  print_asm("leave");
}

make_EHelper(cltd) {
  if (decinfo.isa.is_operand_size_16) {
    //TODO();
    if ((cpu.eax>>15)&1) cpu.edx|=0x0000ffff;
    else cpu.edx&=0xffff0000;
  }
  else {
    //TODO();
    if ((cpu.eax>>31)&1) cpu.edx|=0xffffffff;
    else cpu.edx&=0;
  }
  print_asm(decinfo.isa.is_operand_size_16 ? "cwtl" : "cltd");
}

make_EHelper(cwtl) {
  if (decinfo.isa.is_operand_size_16) {
    //TODO();
    if ((cpu.eax>>7)&1) cpu.eax|=0x0000ff00;
    else cpu.eax&=0xffff00ff;
  }
  else {
    if ((cpu.eax>>15)&1) cpu.eax|=0xffff0000;
    else cpu.eax&=0x0000ffff;
  }
  print_asm(decinfo.isa.is_operand_size_16 ? "cbtw" : "cwtl");
}

make_EHelper(movsx) {
  id_dest->width = decinfo.isa.is_operand_size_16 ? 2 : 4;
  rtl_sext(&s0, &id_src->val, id_src->width);
  operand_write(id_dest, &s0);
  print_asm_template2(movsx);
}

make_EHelper(movzx) {
  id_dest->width = decinfo.isa.is_operand_size_16 ? 2 : 4;
  operand_write(id_dest, &id_src->val);
  print_asm_template2(movzx);
}

make_EHelper(lea) {
  operand_write(id_dest, &id_src->addr);
  print_asm_template2(lea);
}
