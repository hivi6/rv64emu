#!/bin/bash
set -e

riscv64-elf-gcc -march=rv64i -mabi=lp64 -nostdlib \
	-Wl,-e,_start -Wl,-Ttext=0x0 -o tests/uart/uart.elf tests/uart/uart.s

riscv64-elf-objcopy -O binary tests/uart/uart.elf tests/uart/uart.bin

STR="hello"
FINAL=`echo "$STR" | ./build/uart-test tests/uart/uart.bin | xargs`
if [ "$STR" != "$FINAL" ]; then
	echo "FAILED"
else
	echo "PASSED"
fi

