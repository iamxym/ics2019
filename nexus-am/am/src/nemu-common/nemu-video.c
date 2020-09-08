#include <am.h>
#include <amdev.h>
#include <nemu.h>
#include<klib.h>
size_t __am_video_read(uintptr_t reg, void *buf, size_t size) {
  switch (reg) {
    case _DEVREG_VIDEO_INFO: {
      _DEV_VIDEO_INFO_t *info = (_DEV_VIDEO_INFO_t *)buf;
      info->width = 400;
      info->height = 320;
      return sizeof(_DEV_VIDEO_INFO_t);
    }
  }
  return 0;
}

size_t __am_video_write(uintptr_t reg, void *buf, size_t size) {
  switch (reg) {
    case _DEVREG_VIDEO_FBCTL: {
      _DEV_VIDEO_FBCTL_t *ctl = (_DEV_VIDEO_FBCTL_t *)buf;
      
      uint32_t p=0;
	//printf("%u %u %u %u \n",ctl->x,ctl->y,ctl->w,ctl->h);//x->w 400 y->h 320
      //if (ctl->x+(ctl->w)>400||ctl->y+(ctl->h)>320) {printf("%d\n",4396);assert(0);}
      uint32_t *ff;
      
      
      for (int j=ctl->y;j<ctl->y+ctl->h;++j)
      for (int i=ctl->x;i<ctl->x+ctl->w;++i)
	  {
		//printf("%u %x\n",*(ctl->pixels+p),FB_ADDR+i+j*320);
		ff=(uint32_t *)(uintptr_t)(FB_ADDR)+(j*400);
	        ff[i]=ctl->pixels[p];
		++p;
		
	  }
      if (ctl->sync) {
        outl(SYNC_ADDR, 0);
      }
      return size;
    }
  }
  return 0;
}

void __am_vga_init() {
  int i;
  int size = screen_width() * screen_height();
  uint32_t *fb = (uint32_t *)(uintptr_t)FB_ADDR;
  for (i = 0; i < size; i ++) fb[i] = i;
  draw_sync();
}
