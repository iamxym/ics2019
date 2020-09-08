#include <stdint.h> 
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include<sys/types.h>
#include <string.h>
#include "nemu.h"
#include<regex.h>
int init_monitor(int, char *[]);
void ui_mainloop(int);
uint32_t expr(char [],bool *);
void debug_expr()
{
	uint32_t res1=7777,res2;
	static char _expr[65540];
//	int ret=system("gcc -o ./tools/gen-expr/.gen-expr ./tools/gen-expr/gen-expr.c");
//	if (ret==0) puts("ERROR FILE"),assert(0);
//	FILE *fp=popen("./tools/gen-expr/.gen-expr","r");
	FILE *fp=fopen("./tools/gen-expr/input","r");
	bool ok;
	assert(fp!=NULL);
//	for (int _=1;_<=100;++_)
//	{
		while (fscanf(fp,"%u%[^\n]",&res1,_expr)!=EOF)
//		printf("res1=%u\n",res1);
//		if (_==2)printf("s=%s\n",_expr);
		{
//			printf("%s %ld\n",_expr,strlen(_expr));
			res2=expr(_expr,&ok);
			if (ok==1&&res1==res2) puts("4396");
			else {printf("res_std=%u,res_my=%u",res1,res2);break;}
		}
//	}
	fclose(fp);
}
int main(int argc, char *argv[]) {
  /* Initialize the monitor. */
  int is_batch_mode = init_monitor(argc, argv);
//  debug_expr();//zhn:make the expression test

  /* Receive commands from user. */
  ui_mainloop(is_batch_mode);
  return 0;
}
