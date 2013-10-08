#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/user.h>
#include <sys/ptrace.h>
#include <asm/ptrace.h>
#include <signal.h>
#include <malloc.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "global.h"
#include "injector.h"
#include "elf.h"


int OpenLibInchild(char *libPath,pid_t pid);
int LoadLibInChild(int fd,pid_t pid,char *libPath);
int ChildEntry(unsigned int start,pid_t pid);
/*load control program,modify the bin program entry to control program
 */
int ChildStartFromLib(char *libPath ,pid_t pid,int fd[2])
{
	int r;
	int libFd;
	r=MyPtrace(PTRACE_ATTACH,pid,NULL,NULL);
	if(r<0)
	{
#ifdef PRINTF
		printf("ptrace attach error\n");
#endif
		return -1;
	}
	r=WaitSingal(pid,SIGSTOP);	
	if(r<0)
	{
#ifdef LOGMESSAGE
		LogMessage("no singal stop come  error\n");
#endif
		return -1;
	}

	close(fd[0]);
	r=WritePipe(fd[1],"start_child");
	if(r<0)
	{
#ifdef LOGMESSAGE
		LogMessage("write pipe error\n");
#endif
		return -1;
	}
	close(fd[1]);
	r=ptrace(PTRACE_SETOPTIONS,pid,NULL,PTRACE_O_TRACEEXEC);	
	if(r<0)
	{
#ifdef LOGMESSAGE
		LogMessage("ptrace trace exec error\n");
#endif
		return -1;
	}
	r=MyPtrace(PTRACE_CONT,pid,NULL,NULL);	
	if(r<0)
	{
#ifdef LOGMESSAGE
		LogMessage("ptrace_cont to exec error\n");
#endif
		return -1;
	}
	r=WaitSingal(pid,SIGTRAP);
	if(r<0)
	{
#ifdef LOGMESSAGE
		LogMessage("wait singal sigtrap error\n");
#endif
		return -1;
	}
	libFd=OpenLibInChild(libPath,pid);
	if(libFd<0)
	{
#ifdef LOGMESSAGE
		LogMessage("open lib in child error\n");
#endif
		return -1;
	}
	r=LoadLibInChild(libFd,pid,libPath);
	if(r<0)
	{
#ifdef LOGMESSAGE
		LogMessage("load lib in child error\n");
#endif
		return -1;
	}
	r=MyPtrace(PTRACE_CONT,pid,NULL,NULL);	
//	ptrace(PTRACE_SINGLESTEP,pid,NULL,0);
	if(r<0)
	{
#ifdef LOGMESSAGE
		LogMessage("ptrace_cont start child error\n");
#endif
		return -1;
	}
	struct pt_regs regs;
	r=MyPtrace(PTRACE_GETREGS,pid,NULL,&regs);
	if(r<0)
		printf("here\n");
	printf("end:%x\n",regs.ARM_pc);
	return 0;
}


int LoadLibInChild(int fd,pid_t pid,char *libPath)
{	
	int fdLib;
	int r;
	unsigned char *libStart;
	unsigned char *libBase;
	unsigned char *libEnd;
	unsigned int delta,start;
    unsigned int imageSize;
	Elf32_Off phOff;
	Elf32_Half phSize;
	Elf32_Half phNum;
	unsigned int size;
	fdLib=open(libPath,O_RDONLY);
	if(fdLib<0)
		return -1;
	ELFHEADER elfHeader;
	memset(&elfHeader,0,sizeof(ELFHEADER));
	if(r=GetElfEhdr(&elfHeader,fdLib)<0)
		return -1;
	phOff=elfHeader.e_phoff;
	phSize=elfHeader.e_phentsize;
	phNum=elfHeader.e_phnum;
	ELFPHDR elfPhdr[phNum];
	memset(elfPhdr,0,sizeof(ELFPHDR)*phNum);
	size=phSize*phNum;
	GetElfPhdr(elfPhdr,fdLib,size,phOff);
	GetMapAddr(elfPhdr,phNum,&libStart,&libEnd);
	imageSize=libEnd-libStart;
	delta=MapSegment(elfPhdr,&elfHeader,phNum,imageSize,libStart,fd,pid,fdLib);
	if(delta<0)
	{
		printf("run delta\n");
		return -1;
	}
	start=elfHeader.e_entry+delta;
	r=ChildEntry(start,pid);
	if(r<0)
		return -1;
	return 0;
}

int ChildEntry(unsigned int start,pid_t pid)
{
	/*int r;
	unsigned char *pc=(unsigned char*)start;
	unsigned int regPCOffset;
    regPCOffset=offsetof(struct pt_regs,ARM_pc);
	printf("%u\n",pc);
	r=MyPtrace(PTRACE_POKEUSER,pid,(void*)regPCOffset,pc);
	if(r<0)
	{
		return -1;
	}
	*/
	long r;
	struct pt_regs regs;
	r=MyPtrace(PTRACE_GETREGS,pid,NULL,&regs);
	if(r<0){
		return -1;
	}
	regs.ARM_pc=start;
	r=ptrace(PTRACE_SETREGS,pid,NULL,&regs);
	if(r<0)
	{
		return -1;
	}
	printf("start:%x\n",regs.ARM_pc);
   return 0;
}
/*open the control program which is made as a lib.so in the child
 * program space,return the fd in child.
 */
int OpenLibInChild(char *libPath,pid_t pid)
{
	int r;
	unsigned char *pc;
	unsigned int *openCode=(unsigned int*)malloc(MAX_CODE_SIZE*sizeof(int));
	unsigned int codeSize;
	GenerateOpenCode(openCode,&codeSize);/*open() code */
	r=RunCode(openCode,codeSize,pid);
	free(openCode);
	if(r<0)
	{
#ifdef LOGMESSAGE
		LogMessage("run open code  error\n");
#endif
		return -1;
	}
	return r;
}

int WritePipe(int fd,char *buf)
{
	int nwrite=strlen(buf);
	int wirtten=0;
	int n=0;

	/*wite pipe,which is read by pipe in child*/
	while(nwrite>0)
	{
		n=write(fd,buf+wirtten,nwrite);
		if(n<0)
			break;
		wirtten+=n;
		nwrite-=n;
	}
	return 0;
}


int ForkSuspendedChild(char *exe,char **argv,int fd[2])
{
	int r;
	pid_t pid;
	if((pid=fork())==0)
	{
		/*child*/
		char buf[BUF_MAX_SIZE];
		memset(buf,0,BUF_MAX_SIZE);
		close(fd[1]);
		int nread=0;
		int n;

		/*read pipe,if pipe is empty,suspend the child until 
		 * the pipe is writen by parent
		 */
		while((n=read(fd[0],buf+nread,BUF_MAX_SIZE))>0)
		{
			if(n<0)
				break;
			nread+=n;
		}
		buf[nread]='\0';
		close(fd[0]);

		if(strcmp(buf,"start_child")==0)
		{
			execv("/data/work/test.out",argv);
		}

		/*if run here,the execv run error*/
#ifdef LOGMESSAGE
		LogMessage("execv error\n");
#endif
		exit(-1);
	}
	return pid;
}

int CreatePipe(int fd[2])
{
	int r;
	r=pipe(fd);
	return r;
}

int main(int argc,char *argv[])
{
	//add haddle argv code
	//
	int fd[2];/*pipe*/
	int r;
	char libPath[MAX_PATH_NAME];
	char binPath[MAX_PATH_NAME];
	pid_t pid;
	char str[BUF_MAX_SIZE];
	strcpy(libPath,"/data/work/libtest.so");
	strcpy(binPath,"/data/work/test.out");
	/*create pipe,parent thread use write pipe,child use read pipe*/
	r = CreatePipe(fd);
	if (r!=0)
		return -1;

	/*fork suspend child which is suspended by pipe*/
	pid=ForkSuspendedChild(binPath,NULL,fd);
	if(pid<0)
	{
#ifdef LOGMESSAGE
		LogMessage("fork child  error\n");
#endif
		return -1;
	}

	/*load lib in child,and then start the program from lib,
	 *the lib can control the bin program ,the aim of the lib is to 
	 *get the hot path frim bin program
	 * */
	r=ChildStartFromLib(libPath,pid,fd);
	if(r!=0)
	{
#ifdef LOGMESSAGE
		LogMessage("change the entry from bin file program to control program  error\n");
#endif	
		return -1;
	}

	/*parent wait child stop*/
	if(waitpid(pid,NULL,0)!=pid)
	{
#ifdef LogMessage
		LogMessage("waitpid error\n");
#endif
		return -1;
	}
	return 0;
}
