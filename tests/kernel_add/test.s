	.file	"stride.c"
	.text
	.p2align 4,,15
.globl entryPoint
	.type	entryPoint, @function
entryPoint:
.LFB22:
	.cfi_startproc
    sub $112, %rsp
    #Store r10~r13
    mov %r10, 0(%rsp)
    mov %r11, 8(%rsp)
    mov %r12, 16(%rsp)
    mov %r13, 24(%rsp)
    mov %r14, 32(%rsp)
    mov %r15, 40(%rsp)
    mov %rbx, 48(%rsp)
    #Load r10~r12
    mov 120(%rsp), %r10
    mov 128(%rsp), %r11
    mov 136(%rsp), %r12
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
	add %rbx,%rbx 
	#Unrolling, iteration 2 out of 8
	add %rbx,%rbx
	#Unrolling, iteration 3 out of 8
	add %rbx,%rbx
	#Unrolling, iteration 4 out of 8
	add %rbx,%rbx
	#Unrolling, iteration 5 out of 8
	add %rbx,%rbx
	#Unrolling, iteration 6 out of 8
	add %rbx,%rbx
	#Unrolling, iteration 7 out of 8
	add %rbx,%rbx
	#Unrolling, iteration 8 out of 8
	add %rbx,%rbx
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
    mov 48(%rsp), %rbx
    add $112, %rsp

	ret
	.cfi_endproc
.LFE22:
	.size	entryPoint, .-entryPoint
	.ident	"GCC: (Ubuntu 4.4.3-4ubuntu5) 4.4.3"
	.section	.note.GNU-stack,"",@progbits

