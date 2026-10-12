.section .text
.globl _start
.option norvc
.option norelax

_start:
	# Fault must preserve destination register and PC; exact cause is checked.
	addi x1, x0, 1792
	addi x3, x0, 7
	addi x4, x0, 99
	amoswap.w x4, x3, (x1)

	.org 1024
operands:

	.org 1536
target:
	.quad 0
neighbor:
	.quad 0
