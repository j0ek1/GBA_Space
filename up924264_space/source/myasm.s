.THUMB						@ turn on thumb mode
.ALIGN  2					@ align code correctly in memory 
.GLOBL  collFunction		@ name of function goes here


@ ==================================
@ ==================================

.THUMB_FUNC			@ we are about to declare a thumb function
collFunction:		@ function start

push { r4-r7, lr }	@ push r4-r7 and link register onto stack. Your function might use these
					@ registers, so we need to preserve the values just in case!
					@ we don't need to worry about r0-r3 as it is assumed they will be regularly messed up anyway
	
	@ YOUR ASSEMBLY CODE GOES HERE
	@ r0-r3 will automatically contain any parameters sent when calling the function (any more are pushed to stack for you).
	ldrh r1, [r0]
	sub r1, #16
	strh r1, [r0]
	ldrh r1, [r0]
	add r1, #32
	strh r1, [r0,#24]
	ldrh r1, [r0,#2]
	sub r1, #8
	strh r1, [r0,#2]
	ldrh r1, [r0,#2]
	add r1, #24
	strh r1, [r0,#26]

	ldrh r2, [r0,#4]
	sub r2, #16
	strh r2, [r0,#4]
	ldrh r2, [r0,#4]
	add r2, #32
	strh r2, [r0,#28]
	ldrh r2, [r0,#6]
	sub r2, #8
	strh r2, [r0,#6]
	ldrh r2, [r0,#6]
	add r2, #24
	strh r2, [r0,#30]

	ldrh r3, [r0,#8]
	sub r3, #16
	strh r3, [r0,#8]
	ldrh r3, [r0,#8]
	add r3, #32
	strh r3, [r0,#32]
	ldrh r3, [r0,#10]
	sub r3, #8
	strh r3, [r0,#10]
	ldrh r3, [r0,#10]
	add r3, #24
	strh r3, [r0,#34]

	ldrh r4, [r0,#12]
	sub r4, #16
	strh r4, [r0,#12]
	ldrh r4, [r0,#12]
	add r4, #32
	strh r4, [r0,#36]
	ldrh r4, [r0,#14]
	sub r4, #8
	strh r4, [r0,#14]
	ldrh r4, [r0,#14]
	add r4, #24
	strh r4, [r0,#38]

	ldrh r5, [r0,#16]
	sub r5, #16
	strh r5, [r0,#16]
	ldrh r5, [r0,#16]
	add r5, #32
	strh r5, [r0,#40]
	ldrh r5, [r0,#18]
	sub r5, #8
	strh r5, [r0,#18]
	ldrh r5, [r0,#18]
	add r5, #24
	strh r5, [r0,#42]

	ldrh r6, [r0,#20]
	sub r6, #16
	strh r6, [r0,#20]
	ldrh r6, [r0,#20]
	add r6, #32
	strh r6, [r0,#44]
	ldrh r6, [r0,#22]
	sub r6, #8
	strh r6, [r0,#22]
	ldrh r6, [r0,#22]
	add r6, #24
	strh r6, [r0,#46]

pop { r4-r7 }		@ pop first 4 values from stack back into r4-r7, and also
pop { r3 }			@ pop the next value from stack (stored value for lr) into some unused register, e.g. r3 -
					@ we cannot overwrite lr in thumb mode(?) so we have to do it via a normal register
bx r3				@ "branch and exchange" (return) back to C, using the previous value for lr stored in r3

@ ==================================
@ ==================================
