#include "nemu.h"

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <sys/types.h>
#include <regex.h>

enum {
  TK_NOTYPE = 256, TK_NUM,TK_EQ,TK_NEQ,TK_AND,TK_REG,TK_HEX,

  /* TODO: Add more token types */

};

static struct rule {
  char *regex;
  int token_type;
} rules[] = {

  /* TODO: Add more rules.
   * Pay attention to the precedence level of different rules.
   */
  {" +", TK_NOTYPE},    // spaces
  {"\\(",'('},			// (
  {"0\\x[0-9|a-f|A-F]+",TK_HEX},    // 16 number
  {"[0-9]+",TK_NUM},	// 10 number
  {"\\*",'*'},		    // mul
  {"\\/",'/'},			// div
  {"\\-",'-'},          // sub
  {"\\+",'+'},         // plus
  {"==",TK_EQ },	  // equal
  {"!=",TK_NEQ},		//not equal
  {"&&",TK_AND},			//logical and
  {"\\$(eax|ecx|edx|ebx|esp|ebp|esi|edi|ax|cx|dx|bx|sp|bp|si|di|al|cl|dl|bl|ah|ch|dh|bh|pc)",TK_REG},		//
  {"\\)",')'},          // )
};

#define NR_REGEX (sizeof(rules) / sizeof(rules[0]) )

static regex_t re[NR_REGEX] = {};

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
uint32_t isa_reg_str2val(char*, bool*);
void init_regex() {
  int i;
  char error_msg[128];
  int ret;

  for (i = 0; i < NR_REGEX; i ++) {
    ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
    if (ret != 0) {
      regerror(ret, &re[i], error_msg, 128);
      panic("regex compilation failed: %s\n%s", error_msg, rules[i].regex);
    }
  }
}

typedef struct token {
  int type;
  char str[32];
} Token;

static Token tokens[67000] __attribute__((used)) = {};
static int nr_token __attribute__((used))  = 0;

static bool make_token(char *e) {
  int position = 0;
  int i;
  regmatch_t pmatch;

  nr_token = 0;
  while (e[position] != '\0') {
    /* Try all rules one by one. */
    for (i = 0; i < NR_REGEX; i ++) {
      if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
        char *substr_start = e + position;
        int substr_len = pmatch.rm_eo;

//        Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s",
  //          i, rules[i].regex, position, substr_len, substr_len, substr_start);
        position += substr_len;
        /* TODO: Now a new token is recognized with rules[i]. Add codes
         * to record the token in the array `tokens'. For certain types
         * of tokens, some extra actions should be performed.
         */

        switch (rules[i].token_type) {
		 // TK_NOTYPE:break;
		  case TK_NOTYPE:break;
		  case TK_NUM:
		  {
			if (substr_len>32)
			{
				puts("The length of number is too long!");
				return false;
			}
			tokens[nr_token].type='0';
			strncpy(tokens[nr_token].str,substr_start,substr_len);
			tokens[nr_token].str[substr_len]='\0';
			++nr_token;
			break;
		  }
		  case TK_HEX:
		  {
			if (substr_len>32)
			{				
                 puts("The length of number is too long!");
                 return false;
			}
             tokens[nr_token].type='6';
             strncpy(tokens[nr_token].str,substr_start+2,substr_len-2);
             tokens[nr_token].str[substr_len-2]='\0';
             ++nr_token;
             break;
		  }
		  case '(':{tokens[nr_token++].type='(';break;}
		  case '*':{tokens[nr_token++].type='*';break;}
		  case '/':{tokens[nr_token++].type='/';break;}
		  case '-':{tokens[nr_token++].type='-';break;}
          case '+':{tokens[nr_token++].type='+';break;}
		  case TK_EQ:{tokens[nr_token++].type='=';break;}
		  case TK_NEQ:{tokens[nr_token++].type='!';break;}
		  case TK_AND:{tokens[nr_token++].type='&';break;}
		  case TK_REG:
		  {
			 tokens[nr_token].type='r';
             strncpy(tokens[nr_token].str,substr_start+1,substr_len-1);
             tokens[nr_token].str[substr_len-1]='\0';
             ++nr_token;
             break;
		  }
		  case ')':{tokens[nr_token++].type=')';break;}
          default: TODO();
        }

        break;
      }
    }

    if (i == NR_REGEX) {
      printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
      return false;
    }
  }

  return true;
}
bool check_parentheses(int p,int q)
{
	if (tokens[p].type!='('||tokens[q].type!=')') return false;
	int pat=0;
	for (int i=p+1;i<q;++i)
		if (tokens[i].type=='(') ++pat;
		else if (tokens[i].type==')')
		{
			--pat;
			if (pat<0) return false;
		}
	return true;
}
uint32_t hex_cal(char ch)
{
	return (('a'<=ch&&ch<='f')?ch-'a'+10:(('A'<=ch&&ch<='F')?ch-'A'+10:ch-'0'));
}
int opt_pri(int tp)
{
	switch (tp)
	{
		case '&':return 1;
		case '!':return 2;
		case '=':return 2;
		case '+':return 3;
		case '-':return 3;
		case '*':return 4;
		case '/':return 4;
		default:return 0;
	}
}
uint32_t eval(int p, int q,bool *success) {
  if (p > q) {
	*success=false;
	return 0;
    /* Bad expression */
  }
  else if (p == q) {
	 if (tokens[p].type!='0'&&tokens[p].type!='r'&&tokens[p].type!='6')
	 {
		 *success=false;
		 return 0;
	 }
	 uint32_t val=0,len=strlen(tokens[p].str);
	 if (tokens[p].type=='0')
		for (int i=0;i<len;++i) val=val*10+tokens[p].str[i]-'0';
	 else if (tokens[p].type=='6')
		for (int i=0;i<len;++i) val=val*16+hex_cal(tokens[p].str[i]);
	 else
	 {
		 bool suc=false;
		 val=isa_reg_str2val(tokens[p].str,&suc);
		 if (!suc) {*success=false;return 0;}
	 } 
	 return val;
    /* Single token.
     * For now this token should be a number.
     * Return the value of the number.
     */
  }
  else if (check_parentheses(p, q) == true) {
    /* The expression is surrounded by a matched pair of parentheses.
     * If that is the case, just throw away the parentheses.
     */
	bool suc=true;
	uint32_t res=eval(p+1,q-1,&suc);
	if (suc==false) {*success=false;return 0;}
    return res;
  }
  else {
    int op=-1,pat;
	for (int i=p;i<=q;++i)
	if (opt_pri(tokens[i].type)>0)
	{
		pat=0;
		for (int j=p;j<i;++j)
			if (tokens[j].type=='(') ++pat;
			else if (tokens[j].type==')') --pat;
		if (pat!=0) continue;
		if (op==-1||opt_pri(tokens[op].type)>=opt_pri(tokens[i].type)) op=i;
	}
	if (op!=-1)
	{
		bool suc1=true,suc2=true;
	    uint32_t val1 = eval(p, op - 1,&suc1);
		uint32_t val2 = eval(op + 1, q,&suc2);
		if (!suc1||!suc2) {*success=false;return 0;}
		switch (tokens[op].type) {
		case '+': return val1 + val2;
		case '-': return val1-val2;
		case '*': return val1*val2;
		case '/':
		{
			if (val2==0) {*success=false;return 0;}
			return val1/val2;
		}
		case '=':return val1==val2;
		case '!':return val1!=val2;
		case '&':return val1&&val2;
		default:{*success=false;return 0;}
		}
	}
	else if (tokens[p].type!='p') {*success=false;return 0;}
	else
	{
		bool suc=true;
		uint32_t val=eval(p+1,q,&suc);
		if (!suc||val>=PMEM_SIZE) {*success=false;return 0;}
		return pmem[val];
	}
  }
}
uint32_t expr(char *e, bool *success) {
  if (!make_token(e)) {
    *success = false;
    return 0;
  }
  for (int i = 0; i < nr_token; i ++)
  {
	  if (tokens[i].type == '*' && (i == 0 ||(tokens[i-1].type!='0'&&tokens[i-1].type!='6'&&tokens[i-1].type!='r'&&tokens[i-1].type!=')')) ) 
		  tokens[i].type = 'p';//it is a point
  }
/*  for (int i=0;i<nr_token;++i)
   {
       printf("%c ",tokens[i].type);
       if (tokens[i].type=='0'||tokens[i].type=='6'||tokens[i].type=='r') printf(" %s\n",tokens[i].str);
       else puts("");
   }*/
  /* TODO: Insert codes to evaluate the expression. */
  bool suc=1;
  uint32_t res=eval(0,nr_token-1,&suc);
  if (suc==0)
  {
	  *success=false;
	  return 0;
//	  assert(0);//先中断掉
  }
  *success=true;
  return res;
}
