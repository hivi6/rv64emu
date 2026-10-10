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
	
	bus.write(0x1000, 4, 0x00500093);
	assert(bus.read(0x1000, 4).value() == 0x00500093);

	Cpu cpu;

	cpu.writePc(0x99);
	assert(cpu.fetch(bus).error() 
		== Cpu::Exception::INSTRUCTION_ACCESS_FAULT);

	cpu.writePc(0x1000);
	auto raw_inst = cpu.fetch(bus);
	assert(raw_inst);
	assert(*raw_inst == 0x00500093);

	auto inst = cpu.decode(*raw_inst);
	assert(inst);
	assert(inst->op == Cpu::Op::ADDI);
	assert(inst->rd == 1);
	assert(inst->rs1 == 0);
	assert(inst->imm == 5);

	auto cpuException = cpu.execute(*inst, bus);
	assert(cpuException == std::nullopt);
	assert(cpu.readX(1) == 5);
	assert(cpu.readPc() == 0x1004);

	return 0;
}

