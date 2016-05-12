	.text	
	li		$t0, 62
	li		$v0, 11
	move		$a0, $t0
	syscall	
	li		$v0, 5
	syscall	
	move		$t0, $v0
	sw		$t0, _x
	li		$t0, 62
	li		$v0, 11
	move		$a0, $t0
	syscall	
	li		$v0, 5
	syscall	
	move		$t0, $v0
	sw		$t0, _y
	li		$t0, 120
	li		$v0, 11
	move		$a0, $t0
	syscall	
	li		$t0, 61
	li		$v0, 11
	move		$a0, $t0
	syscall	
	lw		$t0, _x
	li		$v0, 1
	move		$a0, $t0
	syscall	
	li		$v0, 4
	la		$a0, L1
	syscall	
	li		$t0, 121
	li		$v0, 11
	move		$a0, $t0
	syscall	
	li		$t0, 61
	li		$v0, 11
	move		$a0, $t0
	syscall	
	lw		$t0, _y
	li		$v0, 1
	move		$a0, $t0
	syscall	
	li		$v0, 4
	la		$a0, L1
	syscall	
L5:
	lw		$t0, _x
	lw		$t1, _y
	beq		$t0, $t1, L2
	lw		$t0, _x
	lw		$t1, _y
	bge		$t0, $t1, L3
	lw		$t0, _y
	lw		$t1, _x
	sub		$t2, $t0, $t1
	sw		$t2, _y
	b		L4
L3:
	lw		$t0, _x
	lw		$t1, _y
	sub		$t2, $t0, $t1
	sw		$t2, _x
L4:
	b		L5
L2:
	li		$t0, 103
	li		$v0, 11
	move		$a0, $t0
	syscall	
	li		$t0, 61
	li		$v0, 11
	move		$a0, $t0
	syscall	
	lw		$t0, _x
	li		$v0, 1
	move		$a0, $t0
	syscall	
	li		$t0, 10
	li		$v0, 11
	move		$a0, $t0
	syscall	
	li		$v0, 10
	syscall	
	.data	
_x:	.word		0
_y:	.word		0
L1:	.asciiz		"\n"
