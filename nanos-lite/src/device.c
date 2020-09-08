#include "common.h"
#include <amdev.h>
extern uint32_t fg_pcb;
extern void change_gcb(uint32_t id);
size_t serial_write(const void *buf, size_t offset, size_t len) {
//  return 0;
	//Log("writing");
	//_yield();//additional
	char *p=(char*)buf;
	for (int i=0;i<len;++i,++p) _putc(*p);
	return len;
}

#define NAME(key) \
  [_KEY_##key] = #key,

static const char *keyname[256] __attribute__((used)) = {
  [_KEY_NONE] = "NONE",
  _KEYS(NAME)
};

size_t events_read(void *buf, size_t offset, size_t len) {
//  return 0;
	//_yield();//additional
	uint32_t key=read_key();
	static char tmp[105] __attribute__((used));
	if (key!=_KEY_NONE)//key event
	{
		//Log("key event");
		if (key&0x8000)
		{
			if (key>=0x8002&&key<=0x8004) change_gcb(key-0x8001);
			sprintf(tmp,"kd %s\n\0",keyname[key^0x8000]);
		}
		else sprintf(tmp,"ku %s\n\0",keyname[key]);
		if (len>strlen(tmp)) len=strlen(tmp);
		strncpy(buf,tmp,len);
	}
	else//time event
	{
		
		uint32_t now=uptime();
		//Log("%d",now);
		//if (now%1000==0) Log("time event");
		sprintf(tmp,"t %d\n\0",now);
		if (len>strlen(tmp)) len=strlen(tmp);
		strncpy(buf,tmp,len);
	}
	return len;
}

static char dispinfo[128] __attribute__((used)) = {};

size_t dispinfo_read(void *buf, size_t offset, size_t len) {
 // return 0;
	memcpy(buf,dispinfo+offset,len);
	return len;
}

size_t fb_write(const void *buf, size_t offset, size_t len) {
  //return 0;
	//_yield();//additional
	int width=screen_width();
	int x=(offset/4)%width,y=(offset/4)/width;
	draw_rect((uint32_t*)buf,x,y,len/4,1);
	return len;
}

size_t fbsync_write(const void *buf, size_t offset, size_t len) {
  //return 0;
  draw_sync();
  return len;
}

void init_device() {
  Log("Initializing devices...");
  _ioe_init();

  // TODO: print the string to array `dispinfo` with the format
  // described in the Navy-apps convention
 /* int fd=fs_open("/proc/dispinfo");
  char tmp[128];
  fs_read(fd,(void*)tmp,128);*/
  int width=screen_width(),height=screen_height();
  sprintf(dispinfo,"WIDTH:%d\nHEIGHT:%d\n",width,height);

}
