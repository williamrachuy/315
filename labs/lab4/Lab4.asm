
.data

ASCII_table:	.byte	48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 65, 66, 67, 68, 69, 70

.text


bintohex:	addi	$sp, $sp, -4		#increase stack pointer for frame pointer
		sw	$fp, 0($sp)		#store frame pointer on stack
		addi	$sp, $sp, -4		#increase stack pointer for storing return address
		sw	$ra, 0($sp)		#store return address
		addi	$sp, $sp, -20		#increase stack pointer for storing function registers, 
						#	to prevent overwriting data in calling environment
		sw	$t0, 16($sp)		#store temp reg $t0 on stack. Furtherst 
		sw	$t1, 12($sp)		#store temp reg $t1 on stack
		sw	$t2, 8($sp)		#store temp reg $t2 on stack
		sw	$t3, 4($sp)		#store temp reg $t3 on stack
		sw	$t4, 0($sp)		#store temp reg $t3 on stack
		or	$fp, $zero, $sp		#use current stack pointer location as new frame pointer val

		#la	$t3, ASCII_table	#load table address		
		or	$t2, $zero, $zero	#temp initial shift amount, and loop count, as zero initial
		or	$t1, $a1, $zero		#temp holder for argument address
		or	$t4, $a0, $zero		#temp holder for binary value
		  
hexgen:		andi	$t0, $t4, 0x0000000F	#mask lower nibble
		la	$t3, ASCII_table	#load table address
		add	$t3, $t3, $t0 		#add offset, which is the lowest nibble value
		lb	$t3, 0($t3)		#load in memory the ASCII value from the table
		add	$t0, $t1, $t2		#add to get address + offset for string
		sb	$t3, 0($t0)		#store byte in string
		#ori	$t0, $zero, 16		#compare value for loop
		srl	$t4, $t4, 4		#shift temp binary variable right 4 bits 
		add	$t2, $t2, 1		#add one byte to string address offset 
		bge	$t2, 8, hexgen		#branch if string location is equal or greater than 8	


		#lw	$fp, 0($sp)		#load frame pointer back from stack
		addi	$sp, $sp, 20
		lw	$t0, 0($sp)
		lw	$t1, -4($sp)
		lw	$t2, -8($sp)
		lw	$t3, -12($sp)
		lw	$t3, -16($sp)
		addi	$sp, $sp, 4
		lw	$ra, 0($sp)
		addi	$sp, $sp, 4
		or	$sp, $fp, $zero
		jr	$ra
		
		
		
		
		
		