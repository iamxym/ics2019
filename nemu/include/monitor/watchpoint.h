#ifndef __WATCHPOINT_H__
#define __WATCHPOINT_H__

#include "common.h"

typedef struct watchpoint {
  int NO,hit_num;
  struct watchpoint *next;

  /* TODO: Add more members if necessary */
  char str[128];//the string of expression
  uint32_t last_val,now_val;
} WP;

#endif
