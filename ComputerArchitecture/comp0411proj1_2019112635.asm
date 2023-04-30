# ------------------------------------------------------------------------------------
# [KNU COMP0411 Computer Architectures] Skeleton code for the 1st project (calculator)
# ------------------------------------------------------------------------------------
.data
input_str: .string ""
.align 2


.text	
# main
main:
	
	jal x1, test #functionality test (please comment this line out if you want console test)
	
	#----TODO-------------------------------------------------------
	#1. read a string from the console

	li a0, 62
	li a7, 11
	ecall
	
	li a0, 32
	li a7, 11
	ecall
	
	#input string
	li a1, 100 # max len
	la a0, input_str
	li a7, 8
	ecall
	
	#string -> char
	
 	la t1, input_str
 	add t0, zero, zero # number store
 	addi t5, zero, -48 
   	
   	check_char: # if ASCII is between 48 and 57 (= number)
   	addi t4, zero, 48 
   	lb a6, (t1)
   	bltu a6, t4, char_add # not a number (<48) 42, 43, 45, 47 is operator
   	addi t4, zero, 57
   	bgtu a6, t4, Exit # a6 > 57 . Exit
	add t2, a6, t5 # a number - 48 
	
	slli t3, t0, 1 
	slli t6, t0, 3 
	add t0, t3, t6 # t0 * ( 2 + 8 ) 
	add t0, t0, t2 
      	beq zero,zero, print_char

      	
   	char_add:
   	addi t4, zero, 43
   	bne a6, t4, char_sub # if not +
   	add a1, zero, zero
   	mv a2, t0 # copy
   	add t0, zero, zero # 0 initialize
   	add a1, zero, zero # 0 	
   	beq zero,zero, print_char
   	
   	char_sub:
   	addi t4, zero, 45
   	bne a6, t4, char_mul # if not -
   	mv a2, t0 # copy
   	add t0, zero, zero 
   	addi a1, zero, 1 # 1
	beq zero,zero, print_char

	char_mul:
	addi t4, zero, 42
	bne a6, t4, char_div # if not *
   	mv a2, t0
   	add t0, zero, zero
   	addi a1, zero, 2 # 2
	beq zero,zero, print_char
	
	char_div:
	addi t4, zero, 47
	bne a6, t4, char_enter # if not / 
	mv a2, t0
   	add t0, zero, zero
   	addi a1, zero, 3 # 3
	beq zero,zero, print_char
	
	
	char_enter: # cumulated string check and print + \n check + calcuation
	addi t4, zero, 3 
	beq a1, t4, char_diventer # if 3(div) 
	mv a3, t0 #cumulated
	li a7, 11
	addi a0, zero, 61 # = print
	ecall	
	li a7, 1 # result print
	jal ra, calc # calc call
	ecall 
	addi t4, zero, 10 # \n check
   	beq a6, t4, Exit # if \n , Exit
	
	char_diventer:
	mv a3, t0 #cumulated
	li a7, 11 
	addi a0, zero, 61 # = print
	ecall	
	li a7, 1 # result print
	jal ra, calc # calc call
	ecall 	
	li a7, 11
	addi a0, zero, 44 # , print
	ecall	
	li a7, 1
	addi a0, a4, 0 # remainder print
	ecall 
	addi t4, zero, 10
   	beq a6, t4, Exit # if \n , Exit
	
	# Exit (93) with code 0
	Exit:
        li a0, 0
        li a7, 93
        ecall
        ebreak

	print_char:
	mv a0, a6
      	li a7, 11
      	ecall
      	addi t1, t1, 1
      	beq zero,zero, check_char

      	
	
	#2. perform arithmetic operations
	#3. print a string to the console to show the computation result
	#---------------------------------------------------------------
	
	# Exit (93) with code 0
#	Exit:
        #li a0, 0
        #li a7, 93
        #ecall
        #ebreak


#---------------------------------------------------------------------------------------------
#name: calc
#func: performs arithmetic operation
#x11(a1): arithmetic operation (0: addition, 1:  subtraction, 2:  multiplication, 3: division)
#x12(a2): the first operand
#x13(a3): the second operand
#x10(a0): return value
#x14(a4): return value (remainder for division operation)
#---------------------------------------------------------------------------------------------
calc:
	#TODO
	
	addi t0, zero, 0 # operator
	add t2, a2, zero # operand1
	add t3, a3, zero # operand2
	bne a1, t0, calc_sub
	calc_add:
	add t1, t2, t3
	add a0, zero, t1
	jalr x0, 0(x1)
	
	calc_sub:
	addi t0, zero, 1
	bne a1, t0, calc_mul
	not t3, t3
	addi t3, t3, 1
	add t1, t2, t3
	add a0, t1, zero
	jalr x0, 0(x1)
	
	calc_mul:
	addi t0, zero, 2
	bne a1, t0, calc_div
	addi t1, zero, 0
	addi t4, zero, 1  #test for LSB
	mul_loop:
	andi t5, t3, 1 # op2 LSB store
	bne t5, t4, mul_not_eq # if op2 LSB = 1
	add t1, t1, t2 # t1 + = op1
	beq zero,zero, mul_shift
	mul_not_eq:
	addi t1, t1, 0 # if op2 LSB = 0 => resume
	beq zero,zero, mul_shift 
	mul_shift:
	slli t2, t2, 1 # op1 left shift
	srli t3, t3, 1 # op2 right shift
	bne t3, zero, mul_loop # op2 != 0 => mul_loop
	add a0, t1, zero # return t1
	jalr x0, 0(x1)
	

	# t2: dividend t3: divisor
	calc_div:
	addi t1, zero, 0
	addi t6, zero, 0
	div_loop:
	not t4, t3
	addi t4, t4, 1
	add t1, t1, t4 # t1 = t1 - divisor
	addi t5, t1, 0 
	srli t5, t5, 31 # t5 = t1 MSB
	andi t5, t5, 1 
	beq t5, zero, is_zero # if t5 = 0
	add t1, t1, t3 # t1 + divisor
	addi t4, t2, 0 # t4 = dividend
	srli t4, t4, 31 # dividend MSB
	andi t4, t4, 1 
	slli t1, t1, 1 
	add t1, t1, t4 # t1 + dividend MSB
	slli t2, t2, 1
	beq zero, zero, div_check
	is_zero:
	addi t4, t2, 0 
	srli t4, t4, 31
	andi t4, t4, 1
	slli t1, t1, 1
	add t1, t1, t4
	slli t2, t2, 1
	addi t2, t2, 1
	beq zero, zero, div_check
	div_check:
	addi t4, zero, 33 
	addi t6, t6, 1
	bne t4, t6, div_loop # t4 != t6
	srli t1, t1, 1
	addi a0, t2, 0 # quoitent
	addi a4, t1, 0 # remainder
	jalr x0, 0(x1)
	
	


.include "common.asm"
