#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include <string.h>

// this should be enough
static char buf[65536];
static char tmp[55];
int len=0;
uint32_t choose(uint32_t p)
{   
    return ((rand()<<16|rand())^rand())%p;
}
void gen_num()
{
	int wei=0;
	uint32_t num=choose(100);
	while (num||wei==0) tmp[++wei]=num%10+'0',num/=10;
	for (int i=wei;i>=1;--i) buf[len++]=tmp[i];
	buf[len++]='u';
}
void gen_rand_op()
{
	switch (choose(4)){
		case 0:{buf[len++]='+';break;}
		case 1:{buf[len++]='-';break;}
		case 2:{buf[len++]='*';break;}
		case 3:{buf[len++]='/';break;}
	}
}
static inline void gen_rand_expr() {
  if (len>=10000) {gen_num();return;}
  if (choose(2)) buf[len++]=' ';
  if (choose(2)) buf[len++]=' ';
  switch (choose(3)) {
    case 0: {gen_num(); break;}
    case 1: {buf[len++]='('; gen_rand_expr(); buf[len++]=')'; break;}
    default: {gen_rand_expr(); gen_rand_op(); gen_rand_expr(); break;}
  }
  if (choose(2)) buf[len++]=' ';
  if (choose(2)) buf[len++]=' ';
}
static char code_buf[65536];
static char *code_format =
"#include <stdio.h>\n"
"int main() { "
"  unsigned result = %s; "
"  unsigned mark=0;	"
"  printf(\"%%u %%u\", result,mark); "
"  return 0; "
"}";

int main(int argc, char *argv[]) {
  int seed = time(0);
  srand(seed);
  int loop = 1;
  if (argc > 1) {
    sscanf(argv[1], "%d", &loop);
  }
  int i;
  for (i = 0; i < loop; i ++) {
	len=0;
    gen_rand_expr();
	buf[len]='\0';
//	printf("%s\n",buf+1);return 0;
    sprintf(code_buf, code_format, buf);

    FILE *fp = fopen("/tmp/.code.c", "w");
    assert(fp != NULL);
    fputs(code_buf, fp);
    fclose(fp);

    int ret = system("gcc  /tmp/.code.c -Werror -o /tmp/.expr");
    if (ret != 0) continue;

    fp = popen("/tmp/.expr", "r");
    assert(fp != NULL);
    int result,_mark=1;
    fscanf(fp, "%d%d", &result,&_mark);
    pclose(fp);
	if (_mark==0)
	{
	    printf("%u ", result);
		for (int o=0;o<len;++o)
			if (buf[o]!='u') putchar(buf[o]);
		puts("");
	}
  }
  return 0;
}
