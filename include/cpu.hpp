#pragma once

#include "common.hpp"
#include "bus.hpp"

namespace rv64emu {

class Cpu {
public:
	enum class Privilege : std::uint8_t {
		USER       = 0b00,
		SUPERVISOR = 0b01,
		RESERVED   = 0b10,
		MACHINE    = 0b11,
	};

	enum class Exception {
		INSTRUCTION_ADDRESS_MISALIGNED,
		INSTRUCTION_ACCESS_FAULT,
		ILLEGAL_INSTRUCTION,
		BREAKPOINT,
		LOAD_ACCESS_FAULT,
		STORE_ACCESS_FAULT,
		ENVIRONMENT_CALL_FROM_UMODE,
		ENVIRONMENT_CALL_FROM_SMODE,
		ENVIRONMENT_CALL_FROM_MMODE,
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
		ADDIW, SLLIW, SRLIW, SRAIW,
		ADDW, SUBW, SLLW, SRLW, SRAW,

		MUL, MULH, MULHSU, MULHU,
		DIV, DIVU, REM, REMU,
		MULW, DIVW, DIVUW, REMW, REMUW,
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
	std::uint64_t readPc() const;

	/**
	 * Read the general register with index value
	 * Trying to read register 0 will return 0
	 *
	 * Parameters:
	 * 	index = register index
	 *
	 * Returns:
	 * 	uint64_t = current value of the register index
	 */
	std::uint64_t readX(size_t index) const;

	/**
	 * Read the csr register with index value
	 *
	 * Parameters:
	 * 	index = register index
	 *
	 * Returns:
	 * 	uint64_t = current value of the register index
	 */
	std::uint64_t readCsr(std::size_t index) const;

	/**
	 * Read the current mode of the cpu
	 *
	 * Returns:
	 *	Privilege = current mode
	 */
	Privilege readMode() const;

	/**
	 * Change the value of the program counter
	 *
	 * Parameters:
	 * 	value = new value of the program counter
	 */
	void writePc(std::uint64_t value);

	/**
	 * Change the register index value
	 * Trying to write register 0 will not cause any write
	 *
	 * Parameters:
	 * 	index = register index
	 * 	value = new value for the register
	 */
	void writeX(std::size_t index, std::uint64_t value);

	/**
	 * Write the csr register with index value
	 *
	 * Parameters:
	 * 	index = register index
	 * 	value    = new value for the register
	 */
	void writeCsr(std::size_t index, std::uint64_t value);

	/**
	 * Write the new mode to the cpu
	 *
	 * Parameters:
	 * 	value = new mode for the cpu
	 */
	void writeMode(Privilege mode);

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

	/**
	 * execute the decoded instruction
	 * If some exception occurs then return Cpu::Exception
	 *
	 * Parameter:
	 * 	inst = decoded instruction
	 * 	bus  = bus for memory access
	 *
	 * Returns:
	 * 	Exception = if any cpu exception
	 */
	std::optional<Exception> execute(Inst inst, Bus &bus);

private:
	std::array<uint64_t, 32>   vX;    // General Register for the cpu
	uint64_t                   vPc;   // Program counter
	std::array<uint64_t, 4096> vCsr;  // Control & Status Register for cpu
	Cpu::Privilege             vMode; // Current priviledge mode

};

};

