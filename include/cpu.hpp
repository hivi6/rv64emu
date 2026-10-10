#pragma once

#include "common.hpp"
#include "bus.hpp"

namespace rv64emu {

class Cpu {
	std::array<uint64_t, 32> vX;  // Register for the cpu
	uint64_t                 vPc; // Program counter

public:
	enum class Exception {
		INSTRUCTION_ACCESS_FAULT,
		ILLEGAL_INSTRUCTION,
	};

	enum class Op {
		INVALID,
		LUI, AUIPC, JAL, JALR,
		BEQ, BNE, BLT, BGE, BLTU, BGEU,
		LB, LH, LW, LD, LBU, LHU, LWU,
		SB, SH, SW, SD,
		ADDI, XORI, ORI, ANDI, SLTIU, SLTI, SLLI, SRLI, SRAI,
		ADD, SUB, SLL, SLT, SLTU, XOR, SRL, SRA, OR, AND,
		FENCE, FENCE_TSO, PAUSE,
		ECALL, EBREAK,
	};

	struct Inst {
		Op op;
	
		std::uint32_t raw;

		std::uint32_t opcode;
		std::uint32_t rd;
		std::uint32_t rs1;
		std::uint32_t rs2;

		std::uint64_t imm;

		std::uint32_t funct3;
		std::uint32_t funct7;
		std::uint32_t shiftType;
		std::uint32_t shiftAmt;
	};
	
	/**
	 * Default construction
	 *
	 * Set the value of registers and programm counter as zero
	 */
	Cpu();
	
	/**
	 * Read the current program counter value
	 *
	 * Returns:
	 *     uint64_t = current instruction pointer value
	 */
	uint64_t readPc() const;

	/**
	 * Read the register index value
	 * Trying to read register 0 will return 0
	 *
	 * Parameters:
	 * 	index = register index
	 *
	 * Returns:
	 * 	uint64_t = current value of the register index
	 */
	uint64_t readX(size_t index) const;

	/**
	 * Change the value of the program counter
	 *
	 * Parameters:
	 * 	value = new value of the program counter
	 */
	void writePc(uint64_t value);

	/**
	 * Change the register index value
	 * Trying to write register 0 will not cause any write
	 *
	 * Parameters:
	 * 	index = register index
	 * 	value = new value for the register
	 */
	void writeX(size_t index, uint64_t value);

	/**
	 * fetch the instruction from the bus
	 * If some exception occurs then return CpuException
	 *
	 * Parameter:
	 * 	bus = Bus from which instruction will be fetch
	 *
	 * Returns:
	 *	uint32_t  = if cpu fetch was successful
	 *	Exception = if some exception occurs
	 */
	std::expected<std::uint32_t, Exception> fetch(Bus &bus);

	/**
	 * decode the instruction from raw bytes
	 * If some exception occurs then return Cpu::Exception
	 *
	 * Parameter:
	 * 	raw = raw instruction
	 *
	 * Returns:
	 * 	Inst      = Decoded instruction
	 * 	Exception = Any cpu exception
	 */
	static std::expected<Inst, Exception> decode(std::uint32_t raw);
};

};

