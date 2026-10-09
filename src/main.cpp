#include <iostream>

#include "cpu.hpp"

int main() {
	rv64emu::Cpu cpu;
	cpu.writeX(0, 10);
	std::cout << cpu.readX(0) << std::endl;

	std::cout << "Hello, World!" << std::endl;
	return 0;
}

