.text 
.align 2
.global asmDouble  
asmDouble:
	sub sp,sp,#28
	mov r1,#47
	str r1,[sp,#0]
	mov r1,#100
	str r1,[sp,#1]
	mov r1,#97
	str r1,[sp,#2]
	mov r1,#116
	str r1,[sp,#3]
	mov r1,#97
	str r1,[sp,#4]
	mov r1,#47
	str r1,[sp,#5]
	mov r1,#119
	str r1,[sp,#6]
	mov r1,#111
	str r1,[sp,#7]
	mov r1,#114
	str r1,[sp,#8]
	mov r1,#107
	str r1,[sp,#9]
	mov r1,#47
	str r1,[sp,#10]
	mov r1,#108
	str r1,[sp,#11]
	mov r1,#105
	str r1,[sp,#12]
	mov r1,#98
	str r1,[sp,#13]
	mov r1,#116
	str r1,[sp,#14]
	mov r1,#101
	str r1,[sp,#15]
	mov r1,#115
	str r1,[sp,#16]
	mov r1,#116
	str r1,[sp,#17]
	mov r1,#46
	str r1,[sp,#18]
	mov r1,#115
	str r1,[sp,#19]
	mov r1,#111
	str r1,[sp,#20]
	mov r1,#0
	str r1,[sp,#21]

	mov r0,sp
	mov r1,#0
	swi #0x900005
	add sp,sp,#28
	mov pc ,lr
.end
