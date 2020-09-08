#include "monitor/monitor.h"
#include "monitor/expr.h"
#include "monitor/watchpoint.h"
#include "nemu.h"

#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>

void cpu_exec(uint64_t);
void isa_reg_display();
void register_pmem(paddr_t);
uint32_t expr(char *e,bool *success);
WP* new_wp();
void wp_display();
void free_wp(int);
/* We use the `readline' library to provide more flexibility to read from stdin. */
static char* rl_gets() {
  static char *line_read = NULL;

  if (line_read) {
    free(line_read);
    line_read = NULL;
  }

  line_read = readline("(nemu) ");

  if (line_read && *line_read) {
    add_history(line_read);
  }

  return line_read;
}

static int cmd_c(char *args) {
  cpu_exec(-1);
  return 0;
}

static int cmd_q(char *args) {
  return -1;
}
static int cmd_si(char *args)
{
	char* arg=strtok(NULL," ");
	uint64_t N=0;
	if (arg==NULL) N=1;
	else sscanf(arg,"%lu",&N);
	cpu_exec(N);
	return 0;
	//some problem
}

static int cmd_info(char *args)
{
	char* arg=strtok(NULL," ");
	if (strlen(arg)==1)
		if (arg[0]=='r') isa_reg_display();
		else if (arg[0]=='w') wp_display();
		else puts("Unknown command");
	else puts("Unknown command");
	return 0;
}
static int cmd_p(char *args)//can't match ' '
{
	bool is_valid;
	uint32_t res=expr(args,&is_valid);
	if (!is_valid)
		printf("Invalid expression\n");
	else
		printf("%u\n",res);
	return 0;
}
static int cmd_x(char *args)
{
	char *arg=strtok(NULL," ");
	uint64_t N;
	uint32_t num,EXPR;
	if (arg==NULL) {printf("Invalid command\n");return 0;}
	sscanf(arg,"%ld",&N);
	bool suc=1;
	arg=strtok(NULL," ");
	EXPR=expr(arg,&suc);
	//printf("EXPR=%d\n",EXPR);
	if (!suc) {printf("Invalid command\n");return 0;}
	for (uint64_t i=0;i<N;)
	{
		printf("0x%lx:		",EXPR+i);
		for (uint64_t j=0;i<N&&j<8;++j,++i)
			num=pmem[EXPR+i*4]+(pmem[EXPR+i*4+1]<<8)+(pmem[EXPR+i*4+2]<<16)+(pmem[EXPR+i*4+3]<<24),
			printf("0x%08x    ",num);
		puts("");
	}
	printf("\n");
//	register_pmem(EXPR);
	return 0;
}
static int cmd_w(char *args)
{
	char *arg=strtok(NULL," ");
	bool suc=true;
	uint32_t val=expr(arg,&suc);
	if (!suc) {puts("Invalid expression");return 0;}
	WP* now=new_wp();
	strcpy(now->str,arg);
	now->now_val=val;
	now->last_val=0;
	printf("Watchpoint %d:%s\n",now->NO,now->str);
	return 0;
}
static int cmd_d(char *args)
{
	char* arg=strtok(NULL," ");                                             
    int N=0;
    if (arg==NULL) assert(0);
    else sscanf(arg,"%d",&N);
	free_wp(N);
	return 0;
}

static int cmd_help(char *args);
static struct {
  char *name;
  char *description;
  int (*handler) (char *);
} cmd_table [] = {
  { "help", "Display informations about all supported commands", cmd_help },
  { "c", "Continue the execution of the program", cmd_c },
  { "q", "Exit NEMU", cmd_q },
  { "si","The format is 'x N' or 'x',and for the function,let the program suspend execution after stepping through N instructions,and N=1 when the latter number is not given",cmd_si},
  {"info","The format is 'info SUBCMD',and for the function,Print the state of program,for example,print the state of register if SUBCMD='r' and print the information of watchpoint if SUBCMD='w'",cmd_info},
  {"p","The format is 'p EXPR',and for the function,get the value of expression'EXPR'",cmd_p},
  {"x","The format is 'x N EXPR',and for the function,make the result as the starting memory address after getting the value of expreesion'EXPR',and print N continuous 4-bytes in hexadecimal",cmd_x},
  {"w","The format is 'w EXPR',and for the function,let the program suspend execution when the value of expression'EXPR' changes",cmd_w},
  {"d","The format is 'd N',and for the function,delete the watchpoint of index N",cmd_d},
  /* TODO: Add more commands */

};

#define NR_CMD (sizeof(cmd_table) / sizeof(cmd_table[0]))

static int cmd_help(char *args) {
  /* extract the first argument */
  char *arg = strtok(NULL, " ");
  int i;

  if (arg == NULL) {
    /* no argument given */
    for (i = 0; i < NR_CMD; i ++) {
      printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
    }
  }
  else {
    for (i = 0; i < NR_CMD; i ++) {
      if (strncmp(arg, cmd_table[i].name,4) == 0) {
        printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
        return 0;
      }
    }
    printf("Unknown command '%s'\n", arg);
  }
  return 0;
}

void ui_mainloop(int is_batch_mode) {
  if (is_batch_mode) {
    cmd_c(NULL);
    return;
  }

  for (char *str; (str = rl_gets()) != NULL; ) {
    char *str_end = str + strlen(str);

    /* extract the first token as the command */
    char *cmd = strtok(str, " ");
    if (cmd == NULL) { continue; }

    /* treat the remaining string as the arguments,
     * which may need further parsing
     */
    char *args = cmd + strlen(cmd) + 1;
    if (args >= str_end) {
      args = NULL;
    }

#ifdef HAS_IOE
    extern void sdl_clear_event_queue(void);
    sdl_clear_event_queue();
#endif

    int i;
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(cmd, cmd_table[i].name) == 0) {
        if (cmd_table[i].handler(args) < 0) { return; }
        break;
      }
    }

    if (i == NR_CMD) { printf("Unknown command '%s'\n", cmd); }
  }
}
