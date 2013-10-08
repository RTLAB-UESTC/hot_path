.text
.align 2
.global asmDouble
asmDouble:
	ldr r0,[pc,#8]
	ldr r1,[pc,#8]
	mov r2,#3
	swi #0x90007d
	.word 1000
	.word 2000
	bkpt #1
	mov pc,lr
.end
