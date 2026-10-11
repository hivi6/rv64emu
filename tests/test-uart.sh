#!/bin/bash
set -euo pipefail

runner="${1:-./build/uart-test}"

riscv64-elf-gcc -march=rv64i -mabi=lp64 -nostdlib \
	-Wl,-e,_start -Wl,-Ttext=0x0 -o tests/uart/uart.elf tests/uart/uart.s

riscv64-elf-objcopy -O binary tests/uart/uart.elf tests/uart/uart.bin

STR="hello"
FINAL=$(printf '%s' "$STR" | "$runner" tests/uart/uart.bin)
if [ "$STR" != "$FINAL" ]; then
	echo "FAILED"
	exit 1
else
	echo "PASSED"
fi
echo

