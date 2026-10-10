#include "cpu.hpp"

namespace rv64emu {

Cpu::Cpu() : vX{}, vPc{} {}

uint64_t Cpu::readPc() const {
	return vPc;
}

uint64_t Cpu::readX(size_t index) const {
	return vX.at(index);
}

void Cpu::writePc(uint64_t value) {
	vPc = value;
}

void Cpu::writeX(size_t index, uint64_t value) {
	// write protect register 0
	if (index == 0) return;

	vX.at(index) = value;
}

std::expected<std::uint32_t, Cpu::Exception> Cpu::fetch(Bus &bus) {
	auto rawInst = bus.read(readPc(), 4);
	if (!rawInst) {
		return std::unexpected(Exception::INSTRUCTION_ACCESS_FAULT);
	}
	return *rawInst;
}

}

