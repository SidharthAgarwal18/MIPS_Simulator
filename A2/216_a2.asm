.text
.globl main

main:
	la $a0,initial_msg
	li $v0,4
	syscall

	li $t5,0				#contains size of stack initially 0
	li $t6,2				#for comparing with t5
	j loop

loop:
	jal getc				#takes input+NULL in char
	lb $t0,char				#transfers 1 byte from char buffer to $t0

	lb $t1,newline			#if newline then finish
	beq $t0,$t1,finish

	lb $t1,plus				#if sign is plus then op1
	beq $t0,$t1,op1
	
	lb $t1,minus			#if sign is minus then op2
	beq $t0,$t1,op2

	lb $t1,cross			#if sign is cross then op3
	beq $t0,$t1,op3

	bgt $t0, 57, unknown_char	#(char is not 0-9,+,-,*)
	blt $t0, 48, unknown_char

	addi $t0,$t0,-48		#to convert char to equivalent int
	subu $sp,$sp,4			#push in stack
	sw	$t0, ($sp)
	addi $t5,$t5,1			#size of stack increased by 1

	j loop

getc:
	li $v0,8				
	la $a0,char				#takes buffer input from user+NULL in char
	li $a1,2				#char is stored in two bytes
	syscall
	jr $ra					#return to line below "jal getc"

op1:						# for +
	blt $t5,$t6, incorrect_exp
	lw $t2, ($sp)			#pop from stack
	addu $sp,$sp,4

	lw $t3, ($sp)			#pop from stack
	addu $sp,$sp,4

	add $t4,$t2,$t3
	j push_in_stack

op2:						# for -
	blt $t5,$t6, incorrect_exp
	lw $t2, ($sp)
	addu $sp,$sp,4

	lw $t3, ($sp)
	addu $sp,$sp,4

	sub $t4, $t3, $t2
	j push_in_stack

op3:						# for *
	blt $t5,$t6, incorrect_exp
	lw $t2, ($sp)
	addu $sp,$sp,4

	lw $t3, ($sp)
	addu $sp,$sp,4

	mul $t4,$t2,$t3
	j push_in_stack

push_in_stack:				#push calculated integer in stack
	subu $sp,$sp,4
	sw	$t4,($sp)
	addi $t5,$t5,-1

	j loop

finish:
	li $t6,1
	bne $t5,$t6, incorrect_exp #if size!=1 then invalid expression

	lw $t2, ($sp)			# in correct input only one elements is left in stack
	addu $sp,$sp,4

	la $a0,final_msg
	li $v0,4
	syscall

	li $v0,1
	move $a0,$t2
	syscall

	li $v0,10
	syscall

unknown_char:
	la $a0,char_error
	li $v0,4
	syscall

	li $v0,10
	syscall

incorrect_exp:
	la $a0,unbalanced_exp
	li $v0,4
	syscall

	li $v0,10
	syscall


.data
	initial_msg: 	.asciiz "Enter your expression:\n"
	final_msg:		.asciiz "Your final output: "
	char_error:		.asciiz "You have entered invalid character"
	unbalanced_exp: .asciiz "You have entered unbalanced postfix expression"
	char:			.space 2
	null:			.asciiz ""
	newline:		.asciiz "\n"
	plus:			.asciiz "+"
	minus:   		.asciiz "-"
	cross:			.asciiz "*"

