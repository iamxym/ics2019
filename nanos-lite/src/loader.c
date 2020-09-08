#include "proc.h"
#include <elf.h>

#ifdef __ISA_AM_NATIVE__
# define Elf_Ehdr Elf64_Ehdr
# define Elf_Phdr Elf64_Phdr
#else
# define Elf_Ehdr Elf32_Ehdr
# define Elf_Phdr Elf32_Phdr
#define DEFAULT_ENTRY 0x40000000
#endif

//typedef size_t off_t;

size_t ramdisk_read(void *buf, size_t offset, size_t len);
size_t get_ramdisk_size();
int fs_open(const char *pathname);
size_t fs_read(int fd, void *buf, size_t len);
int fs_close(int fd);
size_t get_f_size(int fd);
size_t get_disk_offset(int fd);
size_t fs_lseek(int fd, size_t offset, int whence);
void* new_page(size_t nr_page);
int _protect(_AddressSpace *as);
int _map(_AddressSpace *as, void *va, void *pa, int prot);
static uintptr_t loader(PCB *pcb, const char *filename) {//need to do
//  TODO();
	/*Elf_Phdr head_pro;//程序头表
	Elf_Ehdr head_elf;//elf头
	ramdisk_read(&head_elf,0x0,sizeof(Elf_Ehdr));
	size_t siz=head_elf.e_phentsize,cnt=head_elf.e_phnum;
	for (size_t i=0;i<cnt;++i)
	{
		ramdisk_read((&head_pro),i*siz+head_elf.e_phoff,siz);
		if (head_pro.p_type!=PT_LOAD) continue;
		ramdisk_read((uintptr_t*)(head_pro.p_vaddr),head_pro.p_offset,head_pro.p_filesz);
		if (head_pro.p_filesz==head_pro.p_memsz) continue;
		memset((uintptr_t*)(head_pro.p_vaddr+head_pro.p_filesz),0,head_pro.p_memsz-head_pro.p_filesz);
	}//Log("ooo");
	return head_elf.e_entry;*/
	int fd=fs_open(filename);//f_offset=get_disk_offset(fd),seg_size;
	Elf_Phdr head_pro;
	Elf_Ehdr head_elf;
	fs_read(fd,&head_elf,sizeof(Elf_Ehdr));
	void *pa;//*va=(void*)head_elf.e_entry;
	size_t siz=head_elf.e_phentsize,cnt=head_elf.e_phnum;
	uintptr_t now_v,v_addr;
	size_t page_num;
	for (int i=0;i<cnt;++i)
	{
		fs_lseek(fd,i*siz+head_elf.e_phoff,0);
		fs_read(fd,&head_pro,siz);
		if (head_pro.p_type!=PT_LOAD) continue;
		fs_lseek(fd,head_pro.p_offset,0);
		v_addr=head_pro.p_vaddr;
		//if ((head_pro.p_vaddr&0xfff)) assert(0);
		page_num=(head_pro.p_filesz-1)/PGSIZE+1;//申请的页数
		for (int j=0;j<page_num;++j)
		{
			pa=new_page(1);
			if ((uintptr_t)pa&0xfff) assert(0);
			_map(&(pcb->as),(void*)(head_pro.p_vaddr+j*PGSIZE),pa,0);
			//fs_read(fd,(void*)(head_pro.p_vaddr),head_pro.p_filesz);
			if (j<page_num-1) fs_read(fd,pa,PGSIZE);
			else fs_read(fd,pa,head_pro.p_filesz-PGSIZE*j);
		}
		//TODO 是否要清空？
		//maxn=(maxn>head_pro.p_memsz+head_pro.p_memsz?head_pro.p_memsz+head_pro.p_vaddr:maxn);
		//if (maxn&0xfff) maxn=((maxn+0xfff)&0xfffff000);
		//continue;
		v_addr+=page_num*PGSIZE;
		if (head_pro.p_filesz==head_pro.p_memsz) {pcb->max_brk=v_addr;continue;}
		
		int zero_len=head_pro.p_memsz-head_pro.p_filesz;
		if (zero_len<PGSIZE*page_num-head_pro.p_filesz)
			memset((void*)(((uintptr_t)pa)+(head_pro.p_filesz-PGSIZE*(page_num-1))),0,zero_len);
		else
		{
			memset((void*)(((uintptr_t)pa)+(head_pro.p_filesz-PGSIZE*(page_num-1))),0,PGSIZE*page_num-head_pro.p_filesz);
			zero_len-=(PGSIZE*page_num-head_pro.p_filesz);
			now_v=head_pro.p_vaddr+page_num*PGSIZE;
			page_num=(zero_len-1)/PGSIZE+1;
			for (int j=0;j<page_num;++j)
			{
				pa=new_page(1);
				_map(&(pcb->as),(void*)(now_v),pa,0);
				if ((uintptr_t)pa&0xfff) assert(0);
				if (j<page_num-1) memset(pa,0,PGSIZE);
				else memset(pa,0,zero_len);
				now_v+=PGSIZE;
				zero_len-=PGSIZE;
			}
			v_addr+=page_num*PGSIZE;
		}
		pcb->max_brk=v_addr;
		//memset((void*)(((uintptr_t)pa)+head_pro.p_filesz),0,head_pro.p_memsz-head_pro.p_filesz);
	}
	fs_close(fd);
	//current->max_brk=maxn;
	return head_elf.e_entry;
	//以下无分页
	/*Elf_Phdr head_pro;
	Elf_Ehdr head_elf;
	int fd=fs_open(filename),f_offset=get_disk_offset(fd);
	ramdisk_read(&head_elf,f_offset,sizeof(Elf_Ehdr));
	size_t siz=head_elf.e_phentsize,cnt=head_elf.e_phnum;//I don't know whether the offset of elf is true or not!!!!
	for (size_t i=0;i<cnt;++i)
	{
		ramdisk_read((&head_pro),i*siz+head_elf.e_phoff+f_offset,siz);
		if (head_pro.p_type!=PT_LOAD) continue;
		ramdisk_read((void*)(head_pro.p_vaddr),head_pro.p_offset+f_offset,head_pro.p_filesz);
		if (head_pro.p_filesz==head_pro.p_memsz) continue;
		memset((void*)(head_pro.p_vaddr+head_pro.p_filesz),0,head_pro.p_memsz-head_pro.p_filesz);
	}
	fs_close(fd);
	return head_elf.e_entry;*/
}

void naive_uload(PCB *pcb, const char *filename) {
  uintptr_t entry = loader(pcb, filename);
  Log("Jump to entry = %x", entry);
  ((void(*)())entry) ();//Log("4396");
}

void context_kload(PCB *pcb, void *entry) {
  
  _Area stack;
  stack.start = pcb->stack;
  stack.end = stack.start + sizeof(pcb->stack);

  pcb->cp = _kcontext(stack, entry, NULL);
}

void context_uload(PCB *pcb, const char *filename) {
  _protect(&(pcb->as));
  uintptr_t entry = loader(pcb, filename);

  _Area stack;
  stack.start = pcb->stack;
  stack.end = stack.start + sizeof(pcb->stack);

  pcb->cp = _ucontext(&pcb->as, stack, stack, (void *)entry, NULL);
}
