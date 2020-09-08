#include "cpu/exec.h"
#include "cc.h"

make_EHelper(test) {
  rtl_li(&s1,0);
  rtl_set_OF(&s1);
  rtl_set_CF(&s1);
  rtl_and(&s0,&id_dest->val,&id_src->val);
  rtl_update_ZFSF(&s0,id_dest->width);
  print_asm_template2(test);
}

make_EHelper(and) {
  rtl_li(&s1,0);
  rtl_set_OF(&s1);
  rtl_set_CF(&s1);
  rtl_and(&s0,&id_dest->val,&id_src->val);
  //if (id_dest->width != 4)
 //   		rtl_andi(&s0, &s0, 0xffffffffu >> ((4 - id_dest->width) * 8));
  rtl_update_ZFSF(&s0,id_dest->width);
  operand_write(id_dest,&s0);
  print_asm_template2(and);
}

make_EHelper(xor)
{
	rtl_li(&s1,0);
	rtl_set_OF(&s1);
	rtl_set_CF(&s1);
	rtl_xor(&s0,&id_dest->val,&id_src->val);
//	if (id_dest->width != 4)
 //   		rtl_andi(&s0, &s0, 0xffffffffu >> ((4 - id_dest->width) * 8));
	rtl_update_ZFSF(&s0,id_dest->width);
	operand_write(id_dest,&s0);
	print_asm_template2(xor);
}
make_EHelper(or) {
  rtl_li(&s1,0);
  rtl_set_OF(&s1);
  rtl_set_CF(&s1);
  rtl_or(&s0,&id_dest->val,&id_src->val);
  //if (id_dest->width != 4)
    //rtl_andi(&s0, &s0, 0xffffffffu >> ((4 - id_dest->width) * 8));
  rtl_update_ZFSF(&s0,id_dest->width);
  operand_write(id_dest,&s0);
  print_asm_template2(or);
}

make_EHelper(sar) {
  //TODO();
  rtl_sar(&s0,&id_dest->val,&id_src->val);
  rtl_update_ZFSF(&s0,id_dest->width);
  operand_write(id_dest,&s0);
  
  // unnecessary to update CF and OF in NEMU

  print_asm_template2(sar);
}

make_EHelper(shl) {
  //TODO();
  // unnecessary to update CF and OF in NEMU
  rtl_shl(&s0,&id_dest->val,&id_src->val);
  rtl_update_ZFSF(&s0,id_dest->width);
  operand_write(id_dest,&s0);
  print_asm_template2(shl);
}

make_EHelper(shr) {
  //TODO();
  rtl_shr(&s0,&id_dest->val,&id_src->val);
  rtl_update_ZFSF(&s0,id_dest->width);
  operand_write(id_dest,&s0);
  // unnecessary to update CF and OF in NEMU

  print_asm_template2(shr);
}
/*for (int i=1;i<=id_src->val;++i)
  {
    rtl_shri(&s0,&id_dest->val,id_dest->width*8-1);
    rtl_andi(&s0,&s0,1);
    rtl_shli(&s1,&id_dest->val,1);
    rtl_or(&s1,&s1,&s0);
    rtl_get_CF(&s1);
    rtl_or(&s0,&s0,&s1);
  }*/
make_EHelper(rol) {
  if (id_src->val>8*id_dest->width) assert(0);
  rtl_shri(&s0,&id_dest->val,8*id_dest->width-id_src->val);
  rtl_andi(&s0,&s0,(1u<<id_src->val)-1);
  rtl_shli(&s1,&id_dest->val,id_src->val);
  rtl_or(&s1,&s1,&s0);
  operand_write(id_dest,&s1);
  rtl_andi(&s0,&s0,1);
  rtl_set_CF(&s0);
  print_asm_template2(rol);
}

make_EHelper(setcc) {
  uint32_t cc = decinfo.opcode & 0xf;

  rtl_setcc(&s0, cc);
  operand_write(id_dest, &s0);

  print_asm("set%s %s", get_cc_name(cc), id_dest->str);
}

make_EHelper(not) {
//  TODO();
  rtl_not(&s0,&id_dest->val);
  operand_write(id_dest,&s0);
  print_asm_template1(not);
}