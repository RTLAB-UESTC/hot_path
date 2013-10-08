#include <unistd.h>
#include <string.h>
#include <malloc.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/ptrace.h>

#include "elf.h"
#include "global.h"
#include "injector.h"
int GetElfEhdr(ELFHEADER *pElfHeader,int fd)
{
	char *buf=(char*)pElfHeader;
	if(fd<0)
		return -1;
	int nread;
	int toread=sizeof(ELFHEADER);
	while(toread>0)
	{
		nread=read(fd,buf,toread);
		if(nread<0)
			break;
		toread-=nread;
		buf=buf+nread;
	}
	if(toread==0)
	{
		return toread;
	}
	return -1;
}

int GetElfPhdr(ELFPHDR *pElfPhdr,int fd,unsigned size,int start)
{

	char *buf=(char*)pElfPhdr;
	if(fd<0)
		return -1;
	int nread;
	unsigned int toread=size;
	lseek(fd,start,SEEK_SET);
	while(toread>0)
	{
		nread=read(fd,buf,toread);
		if(nread<0)
			break;
		toread-=nread;
		buf=buf+nread;
	}
	if(toread==0)
	{
		return toread;
	}
	return -1;

}

void GetMapAddr(ELFPHDR *pElfPhdr,int numSegment,char **pStart,char **pEnd)
{
	char *minStart,*maxEnd;
	minStart=(char*)ADDR_MAX;
	maxEnd=(char*)ADDR_MIN;
	int i;
	for(i=0;i<numSegment;i++)
	{
		if((pElfPhdr+i)->p_type==LOAD)
		{
			minStart=MIN(minStart,PAGESTART((pElfPhdr+i)->p_vaddr,PAGE_SIZE));
			maxEnd=MAX(maxEnd,PAGEEND((pElfPhdr+i)->p_vaddr+(pElfPhdr+i)->p_memsz,PAGE_SIZE));
		}
	}
	*pStart=minStart;
	*pEnd=maxEnd;
}

unsigned char *MapFunc(unsigned char *addr,unsigned int length,int prot,
		int flags,int fd,unsigned int offset,pid_t pid)
{
	unsigned char *libBase;
	unsigned int *mapCode=(unsigned int*)malloc(MAX_CODE_SIZE*sizeof(int));
	unsigned int codeSize;
	GenerateMapCode(mapCode,&codeSize,addr,length,prot,flags,fd,offset);

	libBase=(unsigned char *)RunCode(mapCode,codeSize,pid);
	free(mapCode);
	return libBase;
}

unsigned char  *ProtFunc(unsigned char *addr,unsigned int length,int prot,pid_t pid)
{
	unsigned char *ret;
	unsigned int *protCode=(unsigned int*)malloc(MAX_CODE_SIZE*sizeof(int));
	unsigned int codeSize;
	GenerateMprotCode(protCode,&codeSize,addr,length,prot);
	ret=RunCode(protCode,codeSize,pid);
	free(protCode);
	return ret;
}


unsigned char  *UnmapFunc(unsigned char *addr,unsigned int length,pid_t pid)
{
	unsigned char *ret;
	unsigned int *unMapCode=(unsigned int*)malloc(MAX_CODE_SIZE*sizeof(int));
	unsigned int codeSize;
	GenerateUnmapCode(unMapCode,&codeSize,addr,length);
	ret=RunCode(unMapCode,codeSize,pid);
	free(unMapCode);
	return ret;
}

unsigned char  *ReadFunc(unsigned char *addr,unsigned int length,pid_t pid,int fd)
{
	unsigned char *ret;
	unsigned int *unMapCode=(unsigned int*)malloc(MAX_CODE_SIZE*sizeof(int));
	unsigned int codeSize;
	GenerateReadCode(unMapCode,&codeSize,fd,addr);
	ret=RunCode(unMapCode,codeSize,pid);
	free(unMapCode);
	return ret;
}
unsigned int MapSegment(ELFPHDR *pElfPhdr,ELFHEADER *pElfHeader,int numSegment,
		unsigned int imageSize,unsigned char *start,int fd,pid_t pid,int libFd)
{
	int r,i;
	unsigned  int delta;
	unsigned char *libBase,*lastEnd,*libEnd;
	libBase=MapFunc(NULL,imageSize,PROT_READ|PROT_WRITE,MAP_PRIVATE|MAP_ANONYMOUS,-1,0,pid);
	libEnd=libBase+imageSize;
    delta=libBase-start;
	lastEnd=libBase;
	for(i=0;i<numSegment;i++)
	{
		unsigned char *segBase,*segEnd;
		unsigned int segSize;
		unsigned int pgOffs;
		ELFPHDR *tempHdr=pElfPhdr+i;
		if(tempHdr->p_type==LOAD)
		{
			segBase=PAGESTART(tempHdr->p_vaddr,PAGE_SIZE)+delta;
			segEnd=PAGEEND(tempHdr->p_vaddr+tempHdr->p_filesz,PAGE_SIZE)+delta;
			segSize=segEnd-segBase;
			if(segBase!=lastEnd)
			{
				unsigned int holeSize=segBase-lastEnd;
				ProtFunc(lastEnd,holeSize,PROT_NONE,pid);
			}
			pgOffs=PAGESTART(tempHdr->p_offset,PAGE_SIZE);
			//UnmapFunc(segBase,segSize,pid);
			MapFunc(segBase,segSize,tempHdr->p_flags|PROT_WRITE|PROT_READ,
					MAP_PRIVATE|MAP_FIXED|MAP_ANONYMOUS,-1,0,pid);
			unsigned char *data=(unsigned char*)malloc(segSize*sizeof(char));
			memset(data,0,segSize);
			lseek(libFd,pgOffs,SEEK_SET);
			unsigned int toRead=segSize;
			unsigned int n,nRead=0;
			while(toRead!=0)
			{
				n=read(libFd,data+nRead,toRead);
				toRead=toRead-n;
				nRead=nRead+n;
			}
			PtraceWriteMemory(pid,(void *)segBase,(void *)data,segSize);
	/*		int i;
			unsigned int *src=(unsigned int *)(data+pElfHeader->e_entry);
			for(i=0;i<10;i++)
				printf("%x\n",src[i]);
				*/
			free(data);
			segEnd=PAGEEND(tempHdr->p_vaddr+tempHdr->p_memsz,PAGE_SIZE)+delta;
			segSize=segEnd-segBase;
			ProtFunc(segBase,segSize,tempHdr->p_flags,pid);
			lastEnd=segEnd;
		}
	}
	return delta;
}

