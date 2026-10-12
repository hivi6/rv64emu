#!/bin/bash
set -uo pipefail

runner="${1:-./build/rv64-test}"
all_passed=1

echo "Testing rv64a"
for test in tests/a/*.s; do
	if riscv64-elf-gcc -march=rv64ia -mabi=lp64 -nostdlib \
		-Wl,-e,_start -Wl,-Ttext=0x0 -o "$test.elf" "$test" \
		&& riscv64-elf-objcopy -O binary "$test.elf" "$test.bin" \
		&& "$runner" "$test.bin" --report-exception 2>&1 | diff - "$test.out"; then
		echo "PASSED - $test"
	else
		echo "FAILED - $test"
		all_passed=0
	fi
done

if [ "$all_passed" = 1 ]; then
	echo PASSED
else
	echo FAILED
	exit 1
fi
echo
