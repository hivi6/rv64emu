.section .text
.globl _start
.option norvc
.option norelax

_start:
	addi x1, x0, 368  # address of the operand table
	# positive operands; expected result = 0x000000000000002a
	ld x2, 0(x1)
	ld x3, 8(x1)
	mulw x4, x2, x3
	# negative lhs; expected result = 0xffffffffffffffd6
	ld x2, 16(x1)
	ld x3, 24(x1)
	mulw x4, x2, x3
	# negative rhs; expected result = 0xffffffffffffffd6
	ld x2, 32(x1)
	ld x3, 40(x1)
	mulw x4, x2, x3
	# both negative; expected result = 0x000000000000002a
	ld x2, 48(x1)
	ld x3, 56(x1)
	mulw x4, x2, x3
	# zero lhs; expected result = 0x0000000000000000
	ld x2, 64(x1)
	ld x3, 72(x1)
	mulw x4, x2, x3
	# zero divisor / multiplier; expected result = 0x0000000000000000
	ld x2, 80(x1)
	ld x3, 88(x1)
	mulw x4, x2, x3
	# both zero; expected result = 0x0000000000000000
	ld x2, 96(x1)
	ld x3, 104(x1)
	mulw x4, x2, x3
	# signed 64-bit overflow boundary; expected result = 0x0000000000000000
	ld x2, 112(x1)
	ld x3, 120(x1)
	mulw x4, x2, x3
	# 64-bit sign bit; expected result = 0x0000000000000000
	ld x2, 128(x1)
	ld x3, 136(x1)
	mulw x4, x2, x3
	# maximum signed 64-bit value; expected result = 0xfffffffffffffffe
	ld x2, 144(x1)
	ld x3, 152(x1)
	mulw x4, x2, x3
	# all-one operands; expected result = 0x0000000000000001
	ld x2, 160(x1)
	ld x3, 168(x1)
	mulw x4, x2, x3
	# unsigned high bit; expected result = 0xfffffffffffffffe
	ld x2, 176(x1)
	ld x3, 184(x1)
	mulw x4, x2, x3
	# signed 32-bit overflow boundary; expected result = 0xffffffff80000000
	ld x2, 192(x1)
	ld x3, 200(x1)
	mulw x4, x2, x3
	# positive word overflow; expected result = 0xfffffffffffffffe
	ld x2, 208(x1)
	ld x3, 216(x1)
	mulw x4, x2, x3
	# negative word; expected result = 0xfffffffffffffffe
	ld x2, 224(x1)
	ld x3, 232(x1)
	mulw x4, x2, x3
	# upper halves ignored by word operations; expected result = 0xffffffff80000000
	ld x2, 240(x1)
	ld x3, 248(x1)
	mulw x4, x2, x3
	# low word of rhs is zero; expected result = 0x0000000000000000
	ld x2, 256(x1)
	ld x3, 264(x1)
	mulw x4, x2, x3
	# nonzero remainder; expected result = 0x000000000000001e
	ld x2, 272(x1)
	ld x3, 280(x1)
	mulw x4, x2, x3
	# negative dividend / remainder; expected result = 0xffffffffffffffe2
	ld x2, 288(x1)
	ld x3, 296(x1)
	mulw x4, x2, x3
	# dividend smaller than divisor; expected result = 0x000000000000001e
	ld x2, 304(x1)
	ld x3, 312(x1)
	mulw x4, x2, x3
	# cross-limb product; expected result = 0x0000000000000000
	ld x2, 320(x1)
	ld x3, 328(x1)
	mulw x4, x2, x3
	# positive dividend and negative divisor; expected result = 0xffffffffffffffe2
	ld x2, 336(x1)
	ld x3, 344(x1)
	mulw x4, x2, x3
	# both negative with nonzero quotient; expected result = 0x000000000000001e
	ld x2, 352(x1)
	ld x3, 360(x1)
	mulw x4, x2, x3
	# all-one quotient without division by zero; expected result = 0xffffffffffffffff
	ld x2, 368(x1)
	ld x3, 376(x1)
	mulw x4, x2, x3
	# rd aliases rs1; expected result = 0xffffffffffffffe2
	ld x2, 384(x1)
	ld x3, 392(x1)
	mulw x2, x2, x3
	# rd aliases rs2; expected result = 0xffffffffffffffe2
	ld x2, 400(x1)
	ld x3, 408(x1)
	mulw x3, x2, x3
	# discard result in x0; expected result = 0xffffffffffffffe2
	ld x2, 416(x1)
	ld x3, 424(x1)
	mulw x0, x2, x3
	# rs1 equals rs2; expected result = 0x0000000000000031
	ld x2, 432(x1)
	ld x3, 440(x1)
	mulw x4, x2, x2
	# x0 as lhs; expected result = 0x0000000000000000
	ld x2, 448(x1)
	ld x3, 456(x1)
	mulw x4, x0, x3
	# x0 as rhs; expected result = 0x0000000000000000
	ld x2, 464(x1)
	ld x3, 472(x1)
	mulw x4, x2, x0
	jal x0, end  # finish at the end of RAM, as in the RV64I tests

data:
	.quad 0x0000000000000006, 0x0000000000000007
	.quad 0xfffffffffffffffa, 0x0000000000000007
	.quad 0x0000000000000006, 0xfffffffffffffff9
	.quad 0xfffffffffffffffa, 0xfffffffffffffff9
	.quad 0x0000000000000000, 0x0000000000000009
	.quad 0x0000000000000009, 0x0000000000000000
	.quad 0x0000000000000000, 0x0000000000000000
	.quad 0x8000000000000000, 0xffffffffffffffff
	.quad 0x8000000000000000, 0x0000000000000002
	.quad 0x7fffffffffffffff, 0x0000000000000002
	.quad 0xffffffffffffffff, 0xffffffffffffffff
	.quad 0xffffffffffffffff, 0x0000000000000002
	.quad 0x0000000080000000, 0xffffffffffffffff
	.quad 0x000000007fffffff, 0x0000000000000002
	.quad 0x00000000ffffffff, 0x0000000000000002
	.quad 0x1234567880000000, 0x87654321ffffffff
	.quad 0x12345678ffffffff, 0x8765432100000000
	.quad 0x000000000000000a, 0x0000000000000003
	.quad 0xfffffffffffffff6, 0x0000000000000003
	.quad 0x0000000000000003, 0x000000000000000a
	.quad 0x0000000100000000, 0x0000000100000000
	.quad 0x000000000000000a, 0xfffffffffffffffd
	.quad 0xfffffffffffffff6, 0xfffffffffffffffd
	.quad 0xffffffffffffffff, 0x0000000000000001
	.quad 0xfffffffffffffff6, 0x0000000000000003
	.quad 0xfffffffffffffff6, 0x0000000000000003
	.quad 0xfffffffffffffff6, 0x0000000000000003
	.quad 0x0000000000000007, 0x0000000000000007
	.quad 0x0000000000000005, 0x0000000000000007
	.quad 0x0000000000000005, 0x0000000000000007
end:
