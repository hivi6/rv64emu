#/bin/bash

echo "Testing rv64i"
all_passed=1

for test in `find tests/i -name '*.s' | sort`; do
	abi=lp64

	riscv64-elf-gcc -march=rv64i -mabi=lp64 -nostdlib \
		-Wl,-e,_start -Wl,-Ttext=0x0 -o $test.elf $test
	riscv64-elf-objcopy -O binary $test.elf $test.bin
	./build/rv64-test $test.bin 2>&1 \
		| diff - $test.out &> /dev/null
	passed=$?

	if [ $passed == 0 ]; then
		echo PASSED - $test
	else
		echo FAILED - $test
		all_passed=0
	fi
done

if [ $all_passed == 1 ]; then
	echo PASSED
else
	echo FAILED
fi
echo

