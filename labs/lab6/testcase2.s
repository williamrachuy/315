.data				# this starts the data area

# This is the second test case for lab5 - loading and decoding instructions


test1:	.word	0x00001234	# first test integer
test2:	.word	0x00001111	# second test integer

.text				# this starts the code area

# This little test program just adds two integers, possibly producing a
# carry or overflow.  It uses addu, in order to avoid an overflow exception.

main:				# this is a label on a line by itself
	lw	$t0, test1	# The first number
	lw	$t1, test2	# The second number

	jal	add_them	# Add the numbers together, returning result

	addi	$t6, $zero, 4	# put a 4 in $t6
	bne	$t6, $zero, second_destiny

	lb	$t0, test1	# get byte at test1
	beq	$t0, $zero, second_destiny

	or	$t0, $zero, $v0	# save $v0 because the HALT cmd wipes it out!
	ori	$v0, $zero, 10		# TRAP HALT
	syscall			# enditol (end it all)

second_destiny: or	$t0, $zero, $v0
	ori	$v0, $zero, 10
	syscall



# add_them - a subroutine (function) that adds the numbers in $t0 and $t1
# returns a result in $v0

add_them: addu	$v0, $t0, $t1	# add the two integers together
	jr	$ra		# and just return

	.end	main		# entry point
