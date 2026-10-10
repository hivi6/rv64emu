.section .text
.globl _start

# UART base address = 0x0
_start:
	addi x1, x0, 0
echo:
	lbu  x2, 0(x1)    # Read a byte
	sb   x2, 0(x1)    # Echo it
	jal  x0, echo

