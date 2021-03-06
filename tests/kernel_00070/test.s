	.file	"stride.c"
	.text
	.p2align 4,,15
.globl entryPoint
	.type	entryPoint, @function
entryPoint:
.LFB22:
	.cfi_startproc
    sub $48, %rsp
    #Store r10~r13
    mov %r10, 0(%rsp)
    mov %r11, 8(%rsp)
    mov %r12, 16(%rsp)
    mov %r13, 24(%rsp)
    mov %r14, 32(%rsp)
    mov %r15, 40(%rsp)
    #Load r10~r12
    mov 56(%rsp), %r10
    mov 64(%rsp), %r11
    mov 72(%rsp), %r12
    #Reset eax, r13
	 xorl	%eax, %eax
    xor %r13, %r13
    #To be safe, remove a bit...
    sub     $4, %rdi
    testq	%rdi, %rdi
	 je	.L3
	 .p2align 4,,10
	 .p2align 3



.L6:

	#Unroll beginning
	#Unrolled factor 8
	#Unrolling, iteration 1 out of 8
	movss 0(%rsi), %xmm0
	#Unrolling, iteration 2 out of 8
	movss %xmm1, 4(%rsi)
	#Unrolling, iteration 3 out of 8
	movss 8(%rsi), %xmm2
	#Unrolling, iteration 4 out of 8
	movss 12(%rsi), %xmm3
	#Unrolling, iteration 5 out of 8
	movss %xmm4, 16(%rsi)
	#Unrolling, iteration 6 out of 8
	movss 20(%rsi), %xmm5
	#Unrolling, iteration 7 out of 8
	movss 24(%rsi), %xmm6
	#Unrolling, iteration 8 out of 8
	movss %xmm7, 28(%rsi)
	#Unroll ending
	#Induction variables
	##Induction variable: 1 , 1 , 1
	add $1, %eax
	##Induction variable: 1 , 32 , 1
	add $32, %rsi
	sub $8, %rdi
	jge .L6

.L3:
    mov 0(%rsp), %r10
    mov 8(%rsp), %r11
    mov 16(%rsp), %r12
    mov 24(%rsp), %r13
    mov 32(%rsp), %r14
    mov 40(%rsp), %r15
    add $48, %rsp

	ret
	.cfi_endproc
.LFE22:
	.size	entryPoint, .-entryPoint
	.ident	"GCC: (Ubuntu 4.4.3-4ubuntu5) 4.4.3"
	.section	.note.GNU-stack,"",@progbits

