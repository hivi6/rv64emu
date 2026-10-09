#include <iostream>

#include "cpu.hpp"
#include "ram.hpp"

int main() {
	rv64emu::Ram ram(0x1000, 1024);
	std::cout << "RAM base: " 
		<< std::hex << "0x" << ram.base() << std::endl;
	std::cout << "RAM size: " << std::dec << ram.size() << std::endl;

	if (ram.write(0x1000, 1, 64)) std::cout << "Write success" << std::endl;
	if (ram.write(0x9999, 1, 64)) std::cout << "Write success" << std::endl;

	std::cout << "Hello, World!" << std::endl;
	return 0;
}

