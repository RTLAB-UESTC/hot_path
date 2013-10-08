#ifndef INJECTOR_H
#define INJECTOR_H

extern int WaitSignal(pid_t pid,int sig);

extern int MyPtrace(int requst,pid_t pid,void *addr,void *data);
extern int PtraceReadMemory(pid_t pid,void *dst,void *src,size_t len);
extern int PtraceWriteMemory(pid_t pid,void *dst,void *src,size_t len);
extern void GenerateMapCode(unsigned int  *code,unsigned int *codeSize,unsigned char *addr,
		unsigned int length,int prot,int flags,int fd,unsigned int offset);
extern unsigned int RunCode(unsigned int *code,unsigned int codeSize,pid_t pid);
extern void GenerateOpenCode(unsigned int *code,unsigned int *codeSize);
extern void GenerateUnmapCode(unsigned int *code,unsigned int *codeSize,unsigned char *addr,unsigned int length);
extern void GenerateMprotCode(unsigned int *code,unsigned int *codeSize,unsigned char *addr,unsigned int length,int prot);

void GenerateReadCode(unsigned int *code,unsigned int *codeSize,int fd,unsigned char *addr);
int PtraceReadMemory(pid_t pid,void *dst,void *src,size_t len);
int PtraceWriteMemory(pid_t pid,void *dst,void *src,size_t len);
#endif
