#include <iostream>

#include "cpu.hpp"
#include "ram.hpp"
#include "bus.hpp"

int main() {
	rv64emu::Bus bus;
	rv64emu::Ram ram(0x1000, 1024);

	bus.attach(ram);
	
	bus.write(0x1000, 1, 42);
	std::cout << bus.read(0x1000, 1).value() << std::endl;

	std::cout << "Hello, World!" << std::endl;

	return 0;
}

