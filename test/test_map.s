.text
.align 2
.global asmDouble
asmDouble:
	sub sp,sp,#8
	mov r3,#-1
	str r3,[sp,#0]
	ldr r3,[pc,#24]
	str r3,[sp,#4]
	ldr r1,[pc,#20]
	ldr r0,[pc,#20]
	mov r2,#3
	mov r3,#34
	swi #0x9000c0
	add sp,sp,#8
	.word 0
	.word 4096 
	.word 1074352128
	mov pc,lr
.end
