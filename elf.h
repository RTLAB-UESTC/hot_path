#ifndef ELF_H
#define ELF_H

#define EI_NIDENT 16
#define ADDR_MAX 0xffffffff/*32BITS*/
#define ADDR_MIN 0x0
#define LOAD 1 

typedef unsigned short Elf32_Half;
typedef unsigned int Elf32_Word;
typedef unsigned int Elf32_Addr;
typedef unsigned int Elf32_Off;

typedef  struct Elf32_Ehdr{
	unsigned char e_ident[EI_NIDENT];
	Elf32_Half e_type; /* ET_EXEC ET_DYN 等 */
	Elf32_Half e_machine;
	Elf32_Word e_version;
	Elf32_Addr e_entry; /* Entry point */
	Elf32_Off e_phoff; 
	Elf32_Off e_shoff; 
	Elf32_Word e_flags;
	Elf32_Half e_ehsize; 
	Elf32_Half e_phentsize; 
	Elf32_Half e_phnum; 
	Elf32_Half e_shentsize; 
	Elf32_Half e_shnum; 
	Elf32_Half e_shstrndx;
} Elf32_Ehdr;

typedef struct Elf32_Phdr{
	Elf32_Word p_type;
	Elf32_Off p_offset; 
	Elf32_Addr p_vaddr; 
	Elf32_Addr p_paddr;
	Elf32_Word p_filesz; 
	Elf32_Word p_memsz; 
	Elf32_Word p_flags;
	Elf32_Word p_align;
} Elf32_Phdr;
#define ELFHEADER Elf32_Ehdr
#define ELFPHDR Elf32_Phdr

int GetElfEhdr(ELFHEADER *pElfHeader,int fd);
int GetElfPhdr(ELFPHDR *pElfPhdr,int fd,unsigned size,int start);
void GetMapAddr(ELFPHDR *pElfPhdr,int numSegment,char **pStart,char **pEnd);
unsigned int MapSegment(ELFPHDR *pELfPhdr,ELFHEADER *pElfHeader,int numSegment,
		unsigned int imageSize,unsigned char *start,int fd,pid_t pid,int libFd);
#endif
