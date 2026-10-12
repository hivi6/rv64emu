.section .text
.globl _start
.option norvc
.option norelax

_start:
	# Fault must preserve destination register and PC; exact cause is checked.
	addi x1, x0, 1537
	addi x3, x0, 7
	addi x4, x0, 99
	lr.w x4, (x1)

	.org 1024
operands:

	.org 1536
target:
	.quad 0
neighbor:
	.quad 0
