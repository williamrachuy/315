
.data

bintohexString:	.space  10
ASCII_table:	.byte	48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 65, 66, 67, 68, 69, 70

.text

main:		or	$a0, $zero, 0x0000308F	#put test hex value in argument 0
		la	$a1, bintohexString	#load address for string
		jal	bintohex		#call bintohex
		ori	$v0, $zero, 4		#move 4 into V0 for print syscall
		or	$a0, $a1, $zero		#move string address into a0 for print
		syscall				#print string
		ori	$v0, $zero, 10		#move 10 into v0 for exit
		syscall				#exit program
		



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
	
		ori	$t2, $zero, 7		#temp initial shift amount, and loop count, as zero initial
		or	$t1, $a1, $zero		#temp holder for argument address
		or	$t4, $a0, $zero		#temp holder for binary value
		  
hexgen:		andi	$t0, $t4, 0x0000000F	#mask lower nibble
		la	$t3, ASCII_table	#load table address
		add	$t3, $t3, $t0 		#add offset, which is the lowest nibble value
		lb	$t3, 0($t3)		#load in memory the ASCII value from the table
		add	$t0, $t1, $t2		#add to get address + offset for string
		sb	$t3, 0($t0)		#store byte in string
		srl	$t4, $t4, 4		#shift temp binary variable right 4 bits 
		addi	$t2, $t2, -1		#subtract one byte to string address offset 
		bne	$t2, -1, hexgen		#branch if string location is equal or greater than 8	

		or	$t3, $zero, $zero	#load zero to store as null terminator
		addi	$t0, $t1, 8		#add to get address + offset for string
		sb	$t3, 0($t0)		#store byte in string		

		or	$sp, $fp, $zero		#set stack pointer to frame pointer
		lw	$t0, 0($sp)		#load back registers
		lw	$t1, -4($sp)		# ^^
		lw	$t2, -8($sp)		# ^^
		lw	$t3, -12($sp)		# ^^
		lw	$t3, -16($sp)		# ^^
		addi	$sp, $sp, 20		#offset stack for registers
		lw	$ra, 0($sp)		#pop return address
		addi	$sp, $sp, 4		#add offset for return address
		lw	$fp, 0($sp)		#load fram pointer from calling environment
		addi	$sp, $sp, 4		#add offset for frame pointer
		jr	$ra			#return to calling environment

		
		
		
