.section .text
.globl _start
.option norvc
.option norelax

_start:
	addi x1, x0, 1536
	addi x10, x0, 1024
	# Load/reserve case 1; verify word sign extension or full doubleword bits.
	ld x2, 0(x10)
	ld x3, 8(x10)
	sd x2, 0(x1)
	lr.w x4, (x1)
	ld x5, 0(x1)
	# Load/reserve case 2; verify word sign extension or full doubleword bits.
	ld x2, 16(x10)
	ld x3, 24(x10)
	sd x2, 0(x1)
	lr.w.aq x4, (x1)
	ld x5, 0(x1)
	# Load/reserve case 3; verify word sign extension or full doubleword bits.
	ld x2, 32(x10)
	ld x3, 40(x10)
	sd x2, 0(x1)
	lr.w.rl x4, (x1)
	ld x5, 0(x1)
	# Load/reserve case 4; verify word sign extension or full doubleword bits.
	ld x2, 48(x10)
	ld x3, 56(x10)
	sd x2, 0(x1)
	lr.w.aqrl x4, (x1)
	ld x5, 0(x1)
	# Load/reserve case 5; verify word sign extension or full doubleword bits.
	ld x2, 64(x10)
	ld x3, 72(x10)
	sd x2, 0(x1)
	lr.w x4, (x1)
	ld x5, 0(x1)
	# Load/reserve case 6; verify word sign extension or full doubleword bits.
	ld x2, 80(x10)
	ld x3, 88(x10)
	sd x2, 0(x1)
	lr.w.aq x4, (x1)
	ld x5, 0(x1)
	# Load/reserve case 7; verify word sign extension or full doubleword bits.
	ld x2, 96(x10)
	ld x3, 104(x10)
	sd x2, 0(x1)
	lr.w.rl x4, (x1)
	ld x5, 0(x1)
	# Load/reserve case 8; verify word sign extension or full doubleword bits.
	ld x2, 112(x10)
	ld x3, 120(x10)
	sd x2, 0(x1)
	lr.w.aqrl x4, (x1)
	ld x5, 0(x1)
	# Load/reserve case 9; verify word sign extension or full doubleword bits.
	ld x2, 128(x10)
	ld x3, 136(x10)
	sd x2, 0(x1)
	lr.w x4, (x1)
	ld x5, 0(x1)
	# Load/reserve case 10; verify word sign extension or full doubleword bits.
	ld x2, 144(x10)
	ld x3, 152(x10)
	sd x2, 0(x1)
	lr.w.aq x4, (x1)
	ld x5, 0(x1)
	# Load/reserve case 11; verify word sign extension or full doubleword bits.
	ld x2, 160(x10)
	ld x3, 168(x10)
	sd x2, 0(x1)
	lr.w.rl x4, (x1)
	ld x5, 0(x1)
	# Load/reserve case 12; verify word sign extension or full doubleword bits.
	ld x2, 176(x10)
	ld x3, 184(x10)
	sd x2, 0(x1)
	lr.w.aqrl x4, (x1)
	ld x5, 0(x1)
	# Load/reserve case 13; verify word sign extension or full doubleword bits.
	ld x2, 192(x10)
	ld x3, 200(x10)
	sd x2, 0(x1)
	lr.w x4, (x1)
	ld x5, 0(x1)
	# Load/reserve case 14; verify word sign extension or full doubleword bits.
	ld x2, 208(x10)
	ld x3, 216(x10)
	sd x2, 0(x1)
	lr.w.aq x4, (x1)
	ld x5, 0(x1)
	# Load/reserve case 15; verify word sign extension or full doubleword bits.
	ld x2, 224(x10)
	ld x3, 232(x10)
	sd x2, 0(x1)
	lr.w.rl x4, (x1)
	ld x5, 0(x1)
	# Load/reserve case 16; verify word sign extension or full doubleword bits.
	ld x2, 240(x10)
	ld x3, 248(x10)
	sd x2, 0(x1)
	lr.w.aqrl x4, (x1)
	ld x5, 0(x1)
	# LR rd aliases address; the reservation retains the original address.
	lr.w x1, (x1)
	addi x1, x0, 1536
	sc.w x6, x3, (x1)
	ld x5, 0(x1)
	# LR into x0 still establishes a reservation.
	lr.w x0, (x1)
	sc.w x6, x3, (x1)
	ld x5, 0(x1)
	# A later LR replaces the reservation on a different address.
	lr.w x4, (x1)
	addi x6, x1, 8
	lr.w x7, (x6)
	sc.w x8, x3, (x1)
	sc.w x9, x3, (x6)
	# Stop explicitly with a breakpoint after the checks.
	ebreak

	.org 1024
operands:
	.quad 0x000000000000000a, 0x0000000000000003
	.quad 0xfffffffffffffff6, 0x0000000000000003
	.quad 0x000000000000000a, 0xfffffffffffffffd
	.quad 0xfffffffffffffff6, 0xfffffffffffffffd
	.quad 0x0000000000000000, 0x0000000000000000
	.quad 0x0000000000000000, 0x0000000000000009
	.quad 0x0000000000000009, 0x0000000000000000
	.quad 0x7fffffffffffffff, 0x0000000000000001
	.quad 0x8000000000000000, 0xffffffffffffffff
	.quad 0xffffffffffffffff, 0x0000000000000001
	.quad 0xffffffffffffffff, 0x0000000000000002
	.quad 0x000000007fffffff, 0x0000000000000001
	.quad 0x0000000080000000, 0xffffffffffffffff
	.quad 0x00000000ffffffff, 0x0000000000000001
	.quad 0x1234567880000000, 0x87654321ffffffff
	.quad 0x12345678ffffffff, 0x8765432100000000

	.org 1536
target:
	.quad 0
neighbor:
	.quad 0
