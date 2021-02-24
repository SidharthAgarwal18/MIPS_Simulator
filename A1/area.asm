.text
.globl main

main:
	li $v0,4	#Print msg1
	la $a0, msg1
	syscall

	li $v0,5	#input n in t0
	syscall
	move $t0,$v0

	li $t1,1	#initialise count=1
	li $t2,0	#initialise sum=0
	li $t9,0	#initialise remainder=0
	blt $t0,$t1 finish #terminate the program if N=0

	li $v0,4	#Print msg2
	la $a0, msg2
	syscall

	li $v0,5	#input x1
	syscall 
	move $t3,$v0

	li $v0,5	#input y1
	syscall
	move $t4,$v0

while:
	beq $t0,$t1 finish
	addi $t1,$t1,1			

	li $v0,5	#input x
	syscall
	move $t5,$v0

	li $v0,5	#input y
	syscall
	move $t6,$v0

	sub $t7,$t5,$t3		#computing (x(i+1)-x(i)) in t7
	add $t8,$t6,$t4		#computing (y(i+1) -y(i)) in t8
	move $t3,$t5		# moving x(i+1) in t3 for next iteration
	move $t4,$t6		#moving y(i+1) in t4 for next iteration

	mul $t5,$t7,$t8		
	li $t8,2
	div $t5,$t8		
	mflo $t6			#quotient of area stored in t6
	mfhi $t7			# remainder of area stored in t7
	add $t9,$t9,$t7		# sum of remainders in t9
	add $t2,$t2,$t6		#sum of areas in t2
	j while
	
finish:
	li $v0,4 #Print msg3
	la $a0,msg3
	syscall

	li $t8,2
	div $t9,$t8			#dividing sum of remainders by 2
	mflo $t3			
	mfhi $t4			#remainder stored in t4
	add $t2,$t2,$t3		#adding quotient to t2
	
	li $t8,1
	li $t7,0
	li $t6,-1

	beq $t4,$t8 final_end_positive_remainder	#final_sum = sum +0.5
	beq $t4,$t6 final_end_negative_remainder	#final_sum = sum -0.5

	li $v0,1
	move $a0,$t2
	syscall

	li $v0,4
	la $a0,msg4
	syscall

	li $t4,0
	li $v0,1
	move $a0,$t4
	syscall

	li $v0,10
	syscall

final_end_positive_remainder:
	blt $t2,$t7 ELSE1
	j PRINT_frac

ELSE1: 
	add $t2,$t2,$t8
	j PRINT_frac

final_end_negative_remainder:
	bge $t2,$t7 ELSE2
	j PRINT_frac

ELSE2: 
	add $t2,$t2,$t6
	j PRINT_frac


PRINT_frac:
	li $v0,1
	move $a0,$t2
	syscall

	li $v0,4
	la $a0,msg4
	syscall

	li $t4,5
	li $v0,1
	move $a0,$t4
	syscall

	li $v0,10
	syscall

.data


msg1 : .asciiz "Enter n = "
msg2 : .asciiz "Enter your x and y alternatively\n"
msg3:  .asciiz "The area under the graph is = "
msg4:  .asciiz "."