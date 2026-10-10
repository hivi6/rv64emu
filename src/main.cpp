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

	auto inst = cpu.decode(0x00500093);
	assert(inst);
	assert(inst->op == Cpu::Op::ADDI);
	assert(inst->rd == 1);
	assert(inst->rs1 == 0);
	assert(inst->imm == 5);

	return 0;
}

