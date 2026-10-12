.section .text
.globl _start
.option norvc
.option norelax

_start:
	addi x1, x0, 1536
	addi x10, x0, 1024
	ld x2, 0(x10)
	ld x3, 8(x10)
	sd x2, 0(x1)
	# SC without LR fails and does not modify memory.
	sc.d x4, x3, (x1)
	ld x5, 0(x1)
	# Successful SC with ordering suffix none; a second SC fails.
	lr.d x6, (x1)
	sc.d x4, x3, (x1)
	ld x5, 0(x1)
	sc.d x4, x3, (x1)
	ld x5, 0(x1)
	# Successful SC with ordering suffix .aq; a second SC fails.
	lr.d x6, (x1)
	sc.d.aq x4, x3, (x1)
	ld x5, 0(x1)
	sc.d x4, x3, (x1)
	ld x5, 0(x1)
	# Successful SC with ordering suffix .rl; a second SC fails.
	lr.d x6, (x1)
	sc.d.rl x4, x3, (x1)
	ld x5, 0(x1)
	sc.d x4, x3, (x1)
	ld x5, 0(x1)
	# Successful SC with ordering suffix .aqrl; a second SC fails.
	lr.d x6, (x1)
	sc.d.aqrl x4, x3, (x1)
	ld x5, 0(x1)
	sc.d x4, x3, (x1)
	ld x5, 0(x1)
	# A mismatched SC clears the reservation.
	lr.d x6, (x1)
	addi x7, x1, 8
	sc.d x4, x3, (x7)
	sc.d x4, x3, (x1)
	ld x5, 0(x1)
	# A mismatched width fails under the emulator's exact-match reservation policy.
	lr.w x6, (x1)
	sc.d x4, x3, (x1)
	sc.w x4, x3, (x1)
	ld x5, 0(x1)
	# SC destination x1: address/source alias or discarded status.
	lr.d x6, (x1)
	sc.d x1, x3, (x1)
	addi x1, x0, 1536
	ld x5, 0(x1)
	ld x3, 8(x10)
	# SC destination x3: address/source alias or discarded status.
	lr.d x6, (x1)
	sc.d x3, x3, (x1)
	addi x1, x0, 1536
	ld x5, 0(x1)
	ld x3, 8(x10)
	# SC destination x0: address/source alias or discarded status.
	lr.d x6, (x1)
	sc.d x0, x3, (x1)
	addi x1, x0, 1536
	ld x5, 0(x1)
	ld x3, 8(x10)
	# Store zero from x0.
	lr.d x6, (x1)
	sc.d x4, x0, (x1)
	ld x5, 0(x1)
	# Stop explicitly with a breakpoint after the checks.
	ebreak

	.org 1024
operands:
	.quad 0x1234567880000000, 0xfedcba9876543210

	.org 1536
target:
	.quad 0
neighbor:
	.quad 0
