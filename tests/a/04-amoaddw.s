.section .text
.globl _start
.option norvc
.option norelax

_start:
	addi x1, x0, 1536
	addi x10, x0, 1024
	# Case 1: old=0x000000000000000a, source=0x0000000000000003; rd returns OLD.
	ld x2, 0(x10)
	ld x3, 8(x10)
	sd x2, 0(x1)
	amoadd.w x4, x3, (x1)
	ld x5, 0(x1)
	# Case 2: old=0xfffffffffffffff6, source=0x0000000000000003; rd returns OLD.
	ld x2, 16(x10)
	ld x3, 24(x10)
	sd x2, 0(x1)
	amoadd.w.aq x4, x3, (x1)
	ld x5, 0(x1)
	# Case 3: old=0x000000000000000a, source=0xfffffffffffffffd; rd returns OLD.
	ld x2, 32(x10)
	ld x3, 40(x10)
	sd x2, 0(x1)
	amoadd.w.rl x4, x3, (x1)
	ld x5, 0(x1)
	# Case 4: old=0xfffffffffffffff6, source=0xfffffffffffffffd; rd returns OLD.
	ld x2, 48(x10)
	ld x3, 56(x10)
	sd x2, 0(x1)
	amoadd.w.aqrl x4, x3, (x1)
	ld x5, 0(x1)
	# Case 5: old=0x0000000000000000, source=0x0000000000000000; rd returns OLD.
	ld x2, 64(x10)
	ld x3, 72(x10)
	sd x2, 0(x1)
	amoadd.w x4, x3, (x1)
	ld x5, 0(x1)
	# Case 6: old=0x0000000000000000, source=0x0000000000000009; rd returns OLD.
	ld x2, 80(x10)
	ld x3, 88(x10)
	sd x2, 0(x1)
	amoadd.w.aq x4, x3, (x1)
	ld x5, 0(x1)
	# Case 7: old=0x0000000000000009, source=0x0000000000000000; rd returns OLD.
	ld x2, 96(x10)
	ld x3, 104(x10)
	sd x2, 0(x1)
	amoadd.w.rl x4, x3, (x1)
	ld x5, 0(x1)
	# Case 8: old=0x7fffffffffffffff, source=0x0000000000000001; rd returns OLD.
	ld x2, 112(x10)
	ld x3, 120(x10)
	sd x2, 0(x1)
	amoadd.w.aqrl x4, x3, (x1)
	ld x5, 0(x1)
	# Case 9: old=0x8000000000000000, source=0xffffffffffffffff; rd returns OLD.
	ld x2, 128(x10)
	ld x3, 136(x10)
	sd x2, 0(x1)
	amoadd.w x4, x3, (x1)
	ld x5, 0(x1)
	# Case 10: old=0xffffffffffffffff, source=0x0000000000000001; rd returns OLD.
	ld x2, 144(x10)
	ld x3, 152(x10)
	sd x2, 0(x1)
	amoadd.w.aq x4, x3, (x1)
	ld x5, 0(x1)
	# Case 11: old=0xffffffffffffffff, source=0x0000000000000002; rd returns OLD.
	ld x2, 160(x10)
	ld x3, 168(x10)
	sd x2, 0(x1)
	amoadd.w.rl x4, x3, (x1)
	ld x5, 0(x1)
	# Case 12: old=0x000000007fffffff, source=0x0000000000000001; rd returns OLD.
	ld x2, 176(x10)
	ld x3, 184(x10)
	sd x2, 0(x1)
	amoadd.w.aqrl x4, x3, (x1)
	ld x5, 0(x1)
	# Case 13: old=0x0000000080000000, source=0xffffffffffffffff; rd returns OLD.
	ld x2, 192(x10)
	ld x3, 200(x10)
	sd x2, 0(x1)
	amoadd.w x4, x3, (x1)
	ld x5, 0(x1)
	# Case 14: old=0x00000000ffffffff, source=0x0000000000000001; rd returns OLD.
	ld x2, 208(x10)
	ld x3, 216(x10)
	sd x2, 0(x1)
	amoadd.w.aq x4, x3, (x1)
	ld x5, 0(x1)
	# Case 15: old=0x1234567880000000, source=0x87654321ffffffff; rd returns OLD.
	ld x2, 224(x10)
	ld x3, 232(x10)
	sd x2, 0(x1)
	amoadd.w.rl x4, x3, (x1)
	ld x5, 0(x1)
	# Case 16: old=0x12345678ffffffff, source=0x8765432100000000; rd returns OLD.
	ld x2, 240(x10)
	ld x3, 248(x10)
	sd x2, 0(x1)
	amoadd.w.aqrl x4, x3, (x1)
	ld x5, 0(x1)
	# Case 17: old=0x5555555555555555, source=0xaaaaaaaaaaaaaaaa; rd returns OLD.
	ld x2, 256(x10)
	ld x3, 264(x10)
	sd x2, 0(x1)
	amoadd.w x4, x3, (x1)
	ld x5, 0(x1)
	# Case 18: old=0xa5a5a5a55a5a5a5a, source=0x5a5a5a5aa5a5a5a5; rd returns OLD.
	ld x2, 272(x10)
	ld x3, 280(x10)
	sd x2, 0(x1)
	amoadd.w.aq x4, x3, (x1)
	ld x5, 0(x1)
	# Destination x1: address/source alias or x0; memory update still occurs.
	ld x2, 288(x10)
	ld x3, 296(x10)
	sd x2, 0(x1)
	amoadd.w x1, x3, (x1)
	addi x1, x0, 1536
	ld x5, 0(x1)
	# Destination x3: address/source alias or x0; memory update still occurs.
	ld x2, 304(x10)
	ld x3, 312(x10)
	sd x2, 0(x1)
	amoadd.w x3, x3, (x1)
	addi x1, x0, 1536
	ld x5, 0(x1)
	# Destination x0: address/source alias or x0; memory update still occurs.
	ld x2, 320(x10)
	ld x3, 328(x10)
	sd x2, 0(x1)
	amoadd.w x0, x3, (x1)
	addi x1, x0, 1536
	ld x5, 0(x1)
	# rs2 equals rs1; use the original address register as the source value.
	ld x2, 336(x10)
	ld x3, 344(x10)
	sd x2, 0(x1)
	amoadd.w x1, x1, (x1)
	addi x1, x0, 1536
	ld x5, 0(x1)
	# x0 as source; update memory and preserve the neighboring doubleword.
	amoadd.w x4, x0, (x1)
	ld x5, 0(x1)
	ld x6, 8(x1)
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
	.quad 0x5555555555555555, 0xaaaaaaaaaaaaaaaa
	.quad 0xa5a5a5a55a5a5a5a, 0x5a5a5a5aa5a5a5a5
	.quad 0xfffffffffffffff6, 0x0000000000000003
	.quad 0xfffffffffffffff6, 0x0000000000000003
	.quad 0xfffffffffffffff6, 0x0000000000000003
	.quad 0x1234567880000000, 0x0000000000000000

	.org 1536
target:
	.quad 0
neighbor:
	.quad 0
