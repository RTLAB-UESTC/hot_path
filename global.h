#ifndef GLOBAL_H
#define GLOBAL_H

#define LOGMESSAGE
#define LogMessage(str) printf(str)
#define MAX_PATH_NAME 256
#define BUF_MAX_SIZE 256
#define MAX_CODE_SIZE 4*1024
#define PAGESTART(x,alignment) (((unsigned int)x) & ~((unsigned int)(alignment-1)))
#define PAGEEND(x,alignment) (((unsigned int)(x)+alignment-1)&~((unsigned int)(alignment-1)))
#define PAGE_SIZE (4*1024)/*4*/
#define MAX(x,y) ((x)>=(y)?(x):(y))
#define MIN(x,y) ((x)<=(y)?(x):(y))
#endif
