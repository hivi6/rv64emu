#include "cpu.hpp"

namespace rv64emu {

// ========================================
// Helper functions declaration
// ========================================

constexpr std::uint64_t signExtend(std::uint64_t value, std::size_t bits);

constexpr std::uint32_t decodeOpcode(std::uint32_t raw);
constexpr std::uint32_t decodeRd(std::uint32_t raw);
constexpr std::uint32_t decodeRs1(std::uint32_t raw);
constexpr std::uint32_t decodeRs2(std::uint32_t raw);
constexpr std::uint32_t decodeFunct3(std::uint32_t raw);
constexpr std::uint32_t decodeFunct7(std::uint32_t raw);

constexpr std::uint64_t decodeIFormatImm(std::uint32_t raw);
constexpr std::uint64_t decodeSFormatImm(std::uint32_t raw);
constexpr std::uint64_t decodeBFormatImm(std::uint32_t raw);
constexpr std::uint64_t decodeUFormatImm(std::uint32_t raw);
constexpr std::uint64_t decodeJFormatImm(std::uint32_t raw);

// ========================================
// Definition of Cpu class
// ========================================

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

std::expected<Cpu::Inst, Cpu::Exception> Cpu::decode(std::uint32_t raw) {
	Cpu::Op op = Cpu::Op::INVALID;

	const auto opcode = decodeOpcode(raw);
	const auto rd     = decodeRd(raw);
	const auto rs1    = decodeRs1(raw);
	const auto rs2    = decodeRs2(raw);

	std::uint64_t imm    = 0;
	std::uint32_t funct3 = 0;

	if (opcode == 0b0110111) {
		imm = decodeUFormatImm(raw);
		op = Op::LUI;
	}
	else if (opcode == 0b0010111) {
		imm = decodeUFormatImm(raw);
		op = Op::AUIPC;
	}
	else if (opcode == 0b1101111) {
		imm = decodeJFormatImm(raw);
		op = Op::JAL;
	}
	else if (opcode == 0b1100111) {
		imm = decodeIFormatImm(raw);
		funct3 = decodeFunct3(raw);

		if (funct3 == 0b000)
			op = Op::JALR;
	}

	if (op == Op::INVALID) {
		return std::unexpected(Exception::ILLEGAL_INSTRUCTION);
	}

	return Inst{
		.op = op,

		.raw = raw,

		.opcode = opcode,
		.rd     = rd,
		.rs1    = rs1,
		.rs2    = rs2,

		.imm    = imm,
		.funct3 = funct3,
	};
}

// ========================================
// Helper functions definition
// ========================================


constexpr std::uint64_t signExtend(std::uint64_t value, std::size_t bits) {
	assert(bits >= 1 && bits <= 64);
	const std::uint64_t signBit = uint64_t{1} << (bits - 1);
	return (value ^ signBit) - signBit;
}

constexpr std::uint32_t decodeOpcode(std::uint32_t raw) {
	return raw & 0b1111111;
}

constexpr std::uint32_t decodeRd(std::uint32_t raw) {
	return (raw >> 7) & 0b11111;
}

constexpr std::uint32_t decodeRs1(std::uint32_t raw) {
	return (raw >> 15) & 0b11111;
}

constexpr std::uint32_t decodeRs2(std::uint32_t raw) {
	return (raw >> 20) & 0b11111;
}

constexpr std::uint32_t decodeFunct3(std::uint32_t raw) {
	return (raw >> 12) & 0b111;
}

constexpr std::uint32_t decodeFunct7(std::uint32_t raw) {
	return (raw >> 25);
}

constexpr std::uint64_t decodeIFormatImm(std::uint32_t raw) {
	return signExtend(raw >> 20, 12);
}

constexpr std::uint64_t decodeSFormatImm(std::uint32_t raw) {
	const uint64_t value = ((raw >> 25) << 5) | ((raw >> 7) & 0b11111);
	return signExtend(value, 12);
}

constexpr std::uint64_t decodeBFormatImm(std::uint32_t raw) {
	const auto imm_11   = (raw >> 7) & 0b1;
	const auto imm_4_1  = (raw >> 8) & 0b1111;
	const auto imm_10_5 = (raw >> 25) & 0b111111;
	const auto imm_12   = (raw >> 31) & 0b1;
	const auto res = (imm_12 << 12) | (imm_11 << 11) 
		| (imm_10_5 << 5) | (imm_4_1 << 1);
	return signExtend(res, 13);

}

constexpr std::uint64_t decodeUFormatImm(std::uint32_t raw) {
	return signExtend(raw & 0xfffff000u, 32);
}

constexpr std::uint64_t decodeJFormatImm(std::uint32_t raw) {
	const auto imm_10_1  = (raw >> 21) & 0b1111111111;
	const auto imm_20    = (raw >> 31) & 0b1;
	const auto imm_11    = (raw >> 20) & 0b1;
	const auto imm_19_12 = (raw >> 12) & 0b11111111;
	const auto res = (imm_20 << 20) | (imm_19_12 << 12)
		| (imm_11 << 11) | (imm_10_1 << 1);
	return signExtend(res, 21);

}

}

