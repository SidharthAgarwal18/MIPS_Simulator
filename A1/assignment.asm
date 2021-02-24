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
	li.s $f1,0.0	#initialise sum=0
	blt $t0,$t1 finish #terminate the program if N<1
	li.s $f2,0.0		#0 set for comarision in future stages

	li $v0,4	#Print msg2
	la $a0, msg2
	syscall

	li $v0,6	#input x1
	syscall 
	mov.s $f3,$f0

	li $v0,6	#input y1
	syscall
	mov.s $f4,$f0

while:
	beq $t0,$t1 finish
	addi $t1,$t1,1			

	li $v0,6	#input x
	syscall
	mov.s $f5,$f0

	li $v0,6	#input y
	syscall
	mov.s $f6,$f0

	mul.s $f9,$f6,$f4	
	c.le.s $f2,$f9
	bc1t ifcase			#if 0<=y(i+1)*y(i) then abs(y(i+1)+y(i)) as both y(i+1) and y(i) have same sign
	bc1f elsecase		#else abs((y(i+1)^2 + y(i)^2)/(y(i+1)-y(i))) as both have opposite sign
	
continue:
	sub.s $f7,$f5,$f3		#computing (x(i+1)-x(i)) in f7
	mov.s $f3,$f5		# moving x(i+1) in f3 for next iteration
	mov.s $f4,$f6		#moving y(i+1) in f4 for next iteration

	mul.s $f5,$f7,$f8		
	li.s $f8,0.5
	mul.s $f6,$f8,$f5
	add.s $f1,$f6,$f1

	j while

finish:
	li $v0,4 #Print msg3
	la $a0,msg3
	syscall

	li $v0,2
	mov.s $f12,$f1
	syscall

	li $v0,10
	syscall

	
ifcase:
	add.s $f8,$f6,$f4	#f8 = y(i+1)+y(i)
	abs.s $f8,$f8		#its mod
	j continue

elsecase:
	mul.s $f10,$f4,$f4		#y(i)^2
	mul.s $f11,$f6,$f6		#y(i+1)^2
	add.s $f11,$f11,$f10	#y(i+1)^2 + y(i)^2
	sub.s $f12,$f6,$f4		#y(i+1)-y(i)
	div.s $f8,$f11,$f12		#divide
	abs.s $f8,$f8			#mod
	j continue

.data
msg1 : .asciiz "Enter n = "
msg2 : .asciiz "Enter your x and y alternatively\n"
msg3 : .asciiz "The area under the graph is = "