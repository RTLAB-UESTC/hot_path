#include <stdio.h> 
extern int asmDouble(void); 
int main() 
{ 
	int r;
	r=asmDouble();
	printf("%d\n",r );
	return 0; 
}
