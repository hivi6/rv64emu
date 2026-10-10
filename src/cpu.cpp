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

constexpr std::uint32_t decodeShiftAmt(std::uint32_t raw);
constexpr std::uint32_t decodeShiftType(std::uint32_t raw);

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

	std::uint64_t imm = 0;

	const auto funct3    = decodeFunct3(raw);
	const auto funct7    = decodeFunct7(raw);
	const auto shiftType = decodeShiftType(raw);
	const auto shiftAmt  = decodeShiftAmt(raw);

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

		if (funct3 == 0b000) op = Op::JALR;
	}
	else if (opcode == 0b1100011) {
		imm = decodeBFormatImm(raw);

		if (funct3 == 0b000)      op = Op::BEQ;
		else if (funct3 == 0b001) op = Op::BNE;
		else if (funct3 == 0b100) op = Op::BLT;
		else if (funct3 == 0b101) op = Op::BGE;
		else if (funct3 == 0b110) op = Op::BLTU;
		else if (funct3 == 0b111) op = Op::BGEU;
	}
	else if (opcode == 0b0000011) {
		imm = decodeIFormatImm(raw);

		if (funct3 == 0b000)      op = Op::LB;
		else if (funct3 == 0b001) op = Op::LH;
		else if (funct3 == 0b010) op = Op::LW;
		else if (funct3 == 0b011) op = Op::LD;
		else if (funct3 == 0b100) op = Op::LBU;
		else if (funct3 == 0b101) op = Op::LHU;
		else if (funct3 == 0b110) op = Op::LWU;
	}
	else if (opcode == 0b0100011) {
		imm = decodeSFormatImm(raw);

		if (funct3 == 0b000)      op = Op::SB;
		else if (funct3 == 0b001) op = Op::SH;
		else if (funct3 == 0b010) op = Op::SW;
		else if (funct3 == 0b011) op = Op::SD;
	}
	else if (opcode == 0b0010011) {
		imm = decodeIFormatImm(raw);
	
		if (funct3 == 0b000)                              op = Op::ADDI;
		else if (funct3 == 0b100)                         op = Op::XORI;
		else if (funct3 == 0b110)                         op = Op::ORI;
		else if (funct3 == 0b111)                         op = Op::ANDI;
		else if (funct3 == 0b011)                         op = Op::SLTIU;
		else if (funct3 == 0b010)                         op = Op::SLTI;
		else if (funct3 == 0b001 && shiftType == 0)       op = Op::SLLI;
		else if (funct3 == 0b101 && shiftType == 0)       op = Op::SRLI;
		else if (funct3 == 0b101 && shiftType == 0b10000) op = Op::SRAI;
	}
	else if (opcode == 0b0110011) {
		if (funct3 == 0b000 && funct7 == 0b0000000)      op = Op::ADD;
		else if (funct3 == 0b000 && funct7 == 0b0100000) op = Op::SUB;
		else if (funct3 == 0b001 && funct7 == 0b0000000) op = Op::SLL;
		else if (funct3 == 0b010 && funct7 == 0b0000000) op = Op::SLT;
		else if (funct3 == 0b011 && funct7 == 0b0000000) op = Op::SLTU;
		else if (funct3 == 0b100 && funct7 == 0b0000000) op = Op::XOR;
		else if (funct3 == 0b101 && funct7 == 0b0000000) op = Op::SRL;
		else if (funct3 == 0b101 && funct7 == 0b0100000) op = Op::SRA;
		else if (funct3 == 0b110 && funct7 == 0b0000000) op = Op::OR;
		else if (funct3 == 0b111 && funct7 == 0b0000000) op = Op::AND;
	}
	else if (opcode == 0b0001111) {
		if (funct3 == 0b000) {
			const auto fm = (raw >> 28);
			const auto pred = (raw >> 24) & 0b1111;
			const auto succ = (raw >> 20) & 0b1111;

			op = Op::FENCE;

			if (fm == 0b1000 && pred == 0b0011 
				&& succ == 0b0011 && rs1 == 0b00000)
				op = Op::FENCE_TSO;
			else if (fm == 0b0000 && pred == 0b0001
				&& succ == 0b0000 && rs1 == 0b00000
				&& rd == 0b0000)
				op = Op::PAUSE;
		}
	}
	else if (opcode == 0b1110011) {
		imm = decodeIFormatImm(raw);

		if (imm == 0b000000000000 && rs1 == 0b00000 &&
			funct3 == 0b000 && rd == 0b00000)
			op = Op::ECALL;
		if (imm == 0b000000000001 && rs1 == 0b00000 &&
			funct3 == 0b000 && rd == 0b00000)
			op = Op::EBREAK;
	}
	else if (opcode == 0b0011011) {
		imm = decodeIFormatImm(raw);

		if (funct3 == 0b000)                             op = Op::ADDIW;
		else if (funct3 == 0b001 && funct7 == 0b0000000) op = Op::SLLIW;
		else if (funct3 == 0b101 && funct7 == 0b0000000) op = Op::SRLIW;
		else if (funct3 == 0b101 && funct7 == 0b0100000) op = Op::SRAIW;
	}
	else if (opcode == 0b0111011) {
		if (funct3 == 0b000 && funct7 == 0b0000000)      op = Op::ADDW;
		else if (funct3 == 0b000 && funct7 == 0b0100000) op = Op::SUBW;
		else if (funct3 == 0b001 && funct7 == 0b0000000) op = Op::SLLW;
		else if (funct3 == 0b101 && funct7 == 0b0000000) op = Op::SRLW;
		else if (funct3 == 0b101 && funct7 == 0b0100000) op = Op::SRAW;
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

		.funct3    = funct3,
		.funct7    = funct7,
		.shiftType = shiftType,
		.shiftAmt  = shiftAmt,
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

constexpr std::uint32_t decodeShiftAmt(std::uint32_t raw) {
	return (raw >> 20) & 0b111111;
}

constexpr std::uint32_t decodeShiftType(std::uint32_t raw) {
	return raw >> 26;
}

}

