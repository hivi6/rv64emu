#include <iostream>

#include "cpu.hpp"
#include "ram.hpp"
#include "bus.hpp"

using namespace std;
using namespace rv64emu;

int main() {
	Ram ram(0x1000, 1024);

	Bus bus;
	bus.attach(ram);
	
	bus.write(0x1000, 1, 42);
	assert(bus.read(0x1000, 1).value() == 42);

	Cpu cpu;

	cpu.writePc(0x1000);
	assert(cpu.fetch(bus).value() == 42);

	cpu.writePc(0x99);
	assert(cpu.fetch(bus).error() 
		== Cpu::Exception::INSTRUCTION_ACCESS_FAULT);

	return 0;
}

