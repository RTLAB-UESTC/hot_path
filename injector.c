#include <unistd.h>
#include <sys/ptrace.h>
#include <asm/ptrace.h>
#include <sys/wait.h>
#include <signal.h>
#include <stddef.h>
#include "injector.h"
#include "global.h"
int MyPtrace(int requst,pid_t pid,void *addr,void *data)
{
	return(ptrace(requst,pid,addr,data));
}


/*wait singal from pid*/
int WaitSingal(pid_t pid,int sig)
{
	int r;
	int status;
	r=waitpid(pid,&status,0);
	if(r<0)
	{
		return -1;
	}
	if(WIFSTOPPED(status))
	{
	//	printf("number:%d",WSTOPSIG(status));
//		if(WSTOPSIG(status)==sig)
			return 0;
	}
	return -1;
}


int PtraceReadMemory(pid_t pid,void *dst,void *src,size_t len)
{
	int i;
	unsigned int r;
	unsigned int *dstReg=dst;
	unsigned int *srcReg=src;
	for(i=0;i<len/sizeof(int);i++)
	{
		r=ptrace(PTRACE_PEEKDATA,pid,&srcReg[i],NULL);
		*(dstReg+i)=r;
	}
	return 0;
}


int PtraceWriteMemory(pid_t pid,void *dst,void *src,size_t len)
{
	int i;
	int r;
	int *dstReg=dst;
	int *srcReg=src;
	for(i=0;i<len/sizeof(int);i++)
	{
		r=ptrace(PTRACE_POKEDATA,pid,&dstReg[i],(void*)srcReg[i]);
		if(r<0)
			return -1;
	}
	return 0;
}

void GenerateOpenCode(unsigned int *code,unsigned int *codeSize)
{
	*codeSize=200;
	unsigned int codeTmp[]={0xe24dd01c,0xe3a0102f,0xe58d1000,0xe3a01064,0xe58d1001,
		0xe3a01061,0xe58d1002,0xe3a01074,0xe58d1003,0xe3a01061,0xe58d1004,0xe3a0102f,
		0xe58d1005,0xe3a01077,0xe58d1006,0xe3a0106f,0xe58d1007,0xe3a01072,0xe58d1008,
		0xe3a0106b,0xe58d1009,0xe3a0102f,0xe58d100a,0xe3a0106c,0xe58d100b,0xe3a01069,
		0xe58d100c,0xe3a01062,0xe58d100d,0xe3a01074,0xe58d100e,0xe3a01065,0xe58d100f,
		0xe3a01073,0xe58d1010,0xe3a01074,0xe58d1011,0xe3a0102e,0xe58d1012,0xe3a01073,
		0xe58d1013,0xe3a0106f,0xe58d1014,0xe3a01000,0xe58d1015,0xe1a0000d,0xe3a01000,
		0xef900005,0xe28dd01c,0xe1200071};
	int i;
	for(i=0;i<(*codeSize)/4;i++)
		code[i]=codeTmp[i];
}

void GenerateReadCode(unsigned int *code,unsigned int *codeSize,
		int fd,unsigned char *addr)
{
	int i;
	*codeSize=28;
	unsigned int codeTmp[]={0xe59f000c,0xe59f100c,0xe3a03064,0xef900003,
	0x0,0x0,0xe1200071};
	codeTmp[4]=fd;
	codeTmp[5]=addr;
	for(i=0;i<(*codeSize)/4;i++)
	{
		printf("code:%x\n",codeTmp[i]);
		code[i]=codeTmp[i];
	}
}
void GenerateMapCode(unsigned int *code,unsigned int *codeSize,unsigned char *addr,unsigned int length,int prot,
		int flags,int fd,unsigned int offset)
{
	int i;
	if(addr==NULL&&fd==-1)
	{
		*codeSize=48;
	   unsigned int codeTmp1[]={0xe24dd008,0xe3e03000,0xe58d3000,0xe3a03000,0xe58d3004,
			0xe59f1014,0xe3a00000,0x0,0x0,0xef9000c0,0xe1200071,0x0};
		codeTmp1[7]=0xe3a02000|prot;
		codeTmp1[8]=0xe3a03000|flags;
		codeTmp1[11]=length;
		for(i=0;i<(*codeSize)/4;i++)
		{
			code[i]=codeTmp1[i];
		}
	}
	else
	{
		*codeSize=60;
		unsigned int codeTmp2[]={0xe24dd008,0x0,0xe58d3000,0xe59f3024,
		0xe58d3004,0xe59f1014,0xe59f0014,0x0,0x0,0xef9000c0,0xe28dd008,0xe1200071,
		0x0,0x0,0x0};
		//codeTmp2[1]=0xe3a03000|fd;
		codeTmp2[1]=0xe3e03000;
		codeTmp2[7]=0xe3a02000|prot;
		codeTmp2[8]=0xe3a03000|flags;
		codeTmp2[12]=length;
		codeTmp2[13]=addr;
		codeTmp2[14]=offset;
		for(i=0;i<(*codeSize)/4;i++)
		{
			code[i]=codeTmp2[i];
		}
	}

}
void GenerateUnmapCode(unsigned int *code,unsigned int *codeSize,unsigned char *addr,unsigned int length)
{
	int i;
	*codeSize=24;
	unsigned int codeTmp[]={0xe59f0004,0xe59f1004,0xef90005b,0x0,0x0,0xe1200071};
	codeTmp[3]=addr;
	codeTmp[4]=length;
	for(i=0;i<(*codeSize)/4;i++)
	{
		code[i]=codeTmp[i];
	}
}

void GenerateMprotCode(unsigned int *code,unsigned int *codeSize,unsigned char *addr,unsigned int length, int prot)
{
	int i;	
	*codeSize=28;
	unsigned int codeTmp[]={0xe59f0008,0xe59f1008,0x0,0xef90007d,0x0,0x0,0xe1200071};
	codeTmp[2]=0xe3a02000|prot;
	codeTmp[4]=addr;
	codeTmp[5]=length;
	for(i=0;i<(*codeSize)/4;i++)
	{
		code[i]=codeTmp[i];
	}
}

unsigned int RunCode(unsigned int *code,unsigned int codeSize,pid_t pid)
{
    int r;
	unsigned int regPCOffset;
	unsigned char *pc;
	struct pt_regs regs;
	struct pt_regs retRegs;
	unsigned char orginCode[MAX_CODE_SIZE];
	r=MyPtrace(PTRACE_GETREGS,pid,NULL,&regs);
	if(r<0)
	{
		return -1;
	}
	pc=(unsigned char*)PAGESTART(regs.ARM_pc,PAGE_SIZE);
	codeSize=PAGEEND(codeSize,4);
	r=PtraceReadMemory(pid,orginCode,pc,codeSize);
	if(r<0)
		return -1;
	r=PtraceWriteMemory(pid,pc,code,codeSize);
	if(r<0)
		return -1;
    regPCOffset=offsetof(struct pt_regs,ARM_pc);
	r=MyPtrace(PTRACE_POKEUSER,pid,(void*)regPCOffset,pc);
	if(r<0)
	{
		return -1;
	}
	r=MyPtrace(PTRACE_CONT,pid,NULL,NULL);	
	if(r<0)
	{
		return -1;
	}
	r=WaitSingal(pid,SIGSTOP);
	if(r<0)
	{
		return -1;
	}
	r=MyPtrace(PTRACE_GETREGS,pid,NULL,&retRegs);
	if(r<0)
		return -1;
	r=PtraceWriteMemory(pid,pc,orginCode,codeSize);
	if(r<0)
		return -1;

	r=MyPtrace(PTRACE_SETREGS,pid,NULL,&regs);
	if(r<0)
		return -1;
	return retRegs.ARM_r0;
}

