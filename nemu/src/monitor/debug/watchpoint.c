#include "monitor/watchpoint.h"
#include "monitor/expr.h"

#define NR_WP 32

static WP wp_pool[NR_WP] = {};
static WP *head = NULL, *free_ = NULL;
static int W_id;
void init_wp_pool() {
  int i;
  for (i = 0; i < NR_WP; i ++) {
    wp_pool[i].NO = i;
	wp_pool[i].hit_num=0;
    wp_pool[i].next = &wp_pool[i + 1];
	wp_pool[i].last_val=wp_pool[i].now_val=0;
  }
  wp_pool[NR_WP - 1].next = NULL;
  W_id=0;
  head = NULL;
  free_ = wp_pool;
}

/* TODO: Implement the functionality of watchpoint */
bool check_wp()
{
	bool _suc,ok=0;
	for (WP *now=head;now!=NULL;now=now->next)
	{
       now->last_val=now->now_val;
       now->now_val=expr(now->str,&_suc);
       if (now->last_val!=now->now_val) ++now->hit_num,ok=1;
	}
	return ok;
}
WP* new_wp()//get a free watchpoint from the list 'free_'
{
	if (free_==NULL)
	{
		assert(0);
		return NULL;
	}
	if (head==NULL)
	{
		head=free_;
		free_=free_->next;
		head->next=NULL;
		head->NO=++W_id;
		return head;
	}
	WP *now=head;
	while (now->next!=NULL) now=now->next;
	now->next=free_;
	
	free_=free_->next;
	now->next->next=NULL;
	now->next->NO=++W_id;
	now->next->hit_num=0;
	return now->next;
}
void free_wp(int N)//make wp free and return to the list 'free_'
{
	WP *now=head,*wp;	
	if (now!=NULL&&now->NO==N)
	{
		head=head->next;
		now->next=free_;
		free_=now;
		return;
	}
	while (now!=NULL&&now->next->NO!=N) now=now->next;
	if (now==NULL)return;
	wp=now->next;
	now->next=wp->next;
	wp->next=free_;
	free_=wp;
}
void wp_display()
{
	if (head==NULL) return;
	WP *now=head;
	while (now!=NULL)
		printf("Num %d : %s=%u\nbreakpoint already hit %d times\n",now->NO,now->str,now->now_val,now->hit_num),now=now->next;
}
