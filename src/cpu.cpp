#include "cpu.hpp"

namespace rv64emu {

Cpu::Cpu() : x{}, pc{} {}

uint64_t Cpu::readPC() const {
	return pc;
}

uint64_t Cpu::readX(size_t index) const {
	return x.at(index);
}

void Cpu::writePC(uint64_t value) {
	pc = value;
}

void Cpu::writeX(size_t index, uint64_t value) {
	// write protect register 0
	if (index == 0) return;

	x.at(index) = value;
}

}

