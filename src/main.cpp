#include <iostream>

#include "cpu.hpp"

int main() {
	rv64emu::Cpu cpu;
	for (int i = 0; i < 32; i++) {
		cpu.writeX(i, 100 + i);
	}
	for (int i = 0; i < 32; i++) {
		std::cout << "x[" << i << "]: " << cpu.readX(i) << std::endl;
	}

	std::cout << "Hello, World!" << std::endl;
	return 0;
}

