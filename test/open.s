.text
.align 2
.global asmDouble
asmDouble:
	ldr r0,[pc,#8]
	ldr r1,[pc,#8]
	mov r3,#100
	swi 0x900003
	.word 3
	.word 4
	bkpt #1
.end
