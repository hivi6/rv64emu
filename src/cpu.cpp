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
constexpr std::uint32_t decodeCsr(std::uint32_t raw);
constexpr std::uint32_t decodeFunct3(std::uint32_t raw);
constexpr std::uint32_t decodeFunct7(std::uint32_t raw);

constexpr std::uint64_t decodeIFormatImm(std::uint32_t raw);
constexpr std::uint64_t decodeSFormatImm(std::uint32_t raw);
constexpr std::uint64_t decodeBFormatImm(std::uint32_t raw);
constexpr std::uint64_t decodeUFormatImm(std::uint32_t raw);
constexpr std::uint64_t decodeJFormatImm(std::uint32_t raw);

constexpr std::uint32_t decodeShiftAmt(std::uint32_t raw);
constexpr std::uint32_t decodeShiftType(std::uint32_t raw);

constexpr std::uint64_t multHighUnsigned(std::uint64_t a, std::uint64_t b);

// ========================================
// Definition of Cpu class
// ========================================

Cpu::Cpu() : vX{}, vPc{}, vCsr{}, vMode{Cpu::Privilege::MACHINE} {}

std::uint64_t Cpu::readPc() const {
	return vPc;
}

std::uint64_t Cpu::readX(std::size_t index) const {
	return vX.at(index);
}

std::uint64_t Cpu::readCsrInternal(std::size_t index) const {
	return vCsr.at(index);
}

std::optional<Cpu::Exception> Cpu::validateCsr(std::size_t index,
	bool writeAccessRequired) const {

	if (index >= vCsr.size()) 
		return Exception::ILLEGAL_INSTRUCTION;

	const auto accessPermission = (index >> 8) & 0b11;
	const auto rwPermission = (index >> 10) & 0b11;

	if (static_cast<std::uint8_t>(readMode()) < accessPermission
		|| (rwPermission == 0b11 && writeAccessRequired))
		return Exception::ILLEGAL_INSTRUCTION;

	return std::nullopt;
}

std::expected<std::uint64_t, Cpu::Exception> Cpu::readCsr(
	std::size_t index) const {

	const auto error = validateCsr(index, false);
	if (error) return std::unexpected(error.value());

	return vCsr.at(index);
}

Cpu::Privilege Cpu::readMode() const {
	return vMode;
}

void Cpu::writePc(std::uint64_t value) {
	vPc = value;
}

void Cpu::writeX(std::size_t index, std::uint64_t value) {
	// write protect register 0
	if (index == 0) return;

	vX.at(index) = value;
}

void Cpu::writeCsrInternal(std::size_t index, std::uint64_t value) {
	vCsr.at(index) = value;
}

std::expected<void, Cpu::Exception> Cpu::writeCsr(std::size_t index, 
	std::uint64_t value) {

	const auto error = validateCsr(index, true);
	if (error) return std::unexpected(error.value());

	writeCsrInternal(index, value);
	return {};
}

void Cpu::writeMode(Cpu::Privilege mode) {
	vMode = mode;
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
	const auto csr    = decodeCsr(raw);

	std::uint64_t imm = 0;

	const auto funct3    = decodeFunct3(raw);
	const auto funct7    = decodeFunct7(raw);
	const auto shiftType = decodeShiftType(raw);
	const auto shiftAmt  = decodeShiftAmt(raw);
	const auto rl        = funct7 & 0b1;
	const auto aq        = (funct7 >> 1) & 0b1;

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
		else if (funct3 == 0b000 && funct7 == 0b0000001) op = Op::MUL;
		else if (funct3 == 0b001 && funct7 == 0b0000001) op = Op::MULH;
		else if (funct3 == 0b010 && funct7 == 0b0000001) op = Op::MULHSU;
		else if (funct3 == 0b011 && funct7 == 0b0000001) op = Op::MULHU;
		else if (funct3 == 0b100 && funct7 == 0b0000001) op = Op::DIV;
		else if (funct3 == 0b101 && funct7 == 0b0000001) op = Op::DIVU;
		else if (funct3 == 0b110 && funct7 == 0b0000001) op = Op::REM;
		else if (funct3 == 0b111 && funct7 == 0b0000001) op = Op::REMU;
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

		if (funct3 == 0b001)      op = Op::CSRRW;
		else if (funct3 == 0b010) op = Op::CSRRS;
		else if (funct3 == 0b011) op = Op::CSRRC;
		else if (funct3 == 0b101) op = Op::CSRRWI;
		else if (funct3 == 0b110) op = Op::CSRRSI;
		else if (funct3 == 0b111) op = Op::CSRRCI;
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
		else if (funct3 == 0b000 && funct7 == 0b0000001) op = Op::MULW;
		else if (funct3 == 0b100 && funct7 == 0b0000001) op = Op::DIVW;
		else if (funct3 == 0b101 && funct7 == 0b0000001) op = Op::DIVUW;
		else if (funct3 == 0b110 && funct7 == 0b0000001) op = Op::REMW;
		else if (funct3 == 0b111 && funct7 == 0b0000001) op = Op::REMUW;
	}
	else if (opcode == 0b0101111) {
		const auto ext = (funct7 >> 2);
		if (funct3 == 0b010) {
			if (ext == 0b00010 && rs2 == 0) op = Op::LR_W;
			else if (ext == 0b00011)        op = Op::SC_W;
			else if (ext == 0b00001)        op = Op::AMOSWAP_W;
			else if (ext == 0b00000)        op = Op::AMOADD_W;
			else if (ext == 0b00100)        op = Op::AMOXOR_W;
			else if (ext == 0b01100)        op = Op::AMOAND_W;
			else if (ext == 0b01000)        op = Op::AMOOR_W;
			else if (ext == 0b10000)        op = Op::AMOMIN_W;
			else if (ext == 0b10100)        op = Op::AMOMAX_W;
			else if (ext == 0b11000)        op = Op::AMOMINU_W;
			else if (ext == 0b11100)        op = Op::AMOMAXU_W;
		}
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
		.csr    = csr,

		.imm    = imm,

		.funct3    = funct3,
		.funct7    = funct7,
		.shiftType = shiftType,
		.shiftAmt  = shiftAmt,
		.rl        = rl,
		.aq        = aq,
	};
}

std::optional<Cpu::Exception> Cpu::execute(Inst inst, Bus &bus) {
	auto nextPc = readPc() + 4;

	switch (inst.op) {
	case Op::LUI: 
		writeX(inst.rd, inst.imm);
		break;

	case Op::AUIPC:
		writeX(inst.rd, inst.imm + readPc());
		break;

	case Op::JAL:
		nextPc = readPc() + inst.imm;
		if (nextPc % 4 != 0) 
			return Exception::INSTRUCTION_ADDRESS_MISALIGNED;

		writeX(inst.rd, readPc() + 4);
		break;

	case Op::JALR:
		nextPc = (readX(inst.rs1) + inst.imm) & ~std::uint64_t{1};
		if (nextPc % 4 != 0) 
			return Exception::INSTRUCTION_ADDRESS_MISALIGNED;

		writeX(inst.rd, readPc() + 4);
		break;

	case Op::BEQ:
		nextPc = (readX(inst.rs1) == readX(inst.rs2) 
			? readPc() + inst.imm : readPc() + 4);
		if (nextPc % 4 != 0) 
			return Exception::INSTRUCTION_ADDRESS_MISALIGNED;
		break;

	case Op::BNE:
		nextPc = (readX(inst.rs1) != readX(inst.rs2) 
			? readPc() + inst.imm : readPc() + 4);
		if (nextPc % 4 != 0) 
			return Exception::INSTRUCTION_ADDRESS_MISALIGNED;
		break;

	case Op::BLT: {
		const auto lhs = static_cast<int64_t>(readX(inst.rs1));
		const auto rhs = static_cast<int64_t>(readX(inst.rs2));
		nextPc = (lhs < rhs ? readPc() + inst.imm : readPc() + 4);
		if (nextPc % 4 != 0) 
			return Exception::INSTRUCTION_ADDRESS_MISALIGNED;
		break;
	}

	case Op::BGE: {
		const auto lhs = static_cast<int64_t>(readX(inst.rs1));
		const auto rhs = static_cast<int64_t>(readX(inst.rs2));
		nextPc = (lhs >= rhs ? readPc() + inst.imm : readPc() + 4);
		if (nextPc % 4 != 0) 
			return Exception::INSTRUCTION_ADDRESS_MISALIGNED;
		break;
	}

	case Op::BLTU: {
		const auto lhs = (readX(inst.rs1));
		const auto rhs = (readX(inst.rs2));
		nextPc = (lhs < rhs ? readPc() + inst.imm : readPc() + 4);
		if (nextPc % 4 != 0) 
			return Exception::INSTRUCTION_ADDRESS_MISALIGNED;
		break;
	}

	case Op::BGEU: {
		const auto lhs = (readX(inst.rs1));
		const auto rhs = (readX(inst.rs2));
		nextPc = (lhs >= rhs ? readPc() + inst.imm : readPc() + 4);
		if (nextPc % 4 != 0) 
			return Exception::INSTRUCTION_ADDRESS_MISALIGNED;
		break;
	}

	case Op::LB: {
		const auto addr = readX(inst.rs1) + inst.imm;
		const auto busLoad = bus.read(addr, 1);
		if (!busLoad)
			return Exception::LOAD_ACCESS_FAULT;
		
		auto res = signExtend(*busLoad, 8);
		writeX(inst.rd, res);
		break;
	}

	case Op::LH: {
		const auto addr = readX(inst.rs1) + inst.imm;
		const auto busLoad = bus.read(addr, 2);
		if (!busLoad)
			return Exception::LOAD_ACCESS_FAULT;
		
		auto res = signExtend(*busLoad, 16);
		writeX(inst.rd, res);
		break;
	}

	case Op::LW: {
		const auto addr = readX(inst.rs1) + inst.imm;
		const auto busLoad = bus.read(addr, 4);
		if (!busLoad)
			return Exception::LOAD_ACCESS_FAULT;
		
		auto res = signExtend(*busLoad, 32);
		writeX(inst.rd, res);
		break;
	}

	case Op::LD: {
		const auto addr = readX(inst.rs1) + inst.imm;
		const auto busLoad = bus.read(addr, 8);
		if (!busLoad)
			return Exception::LOAD_ACCESS_FAULT;
		
		auto res = signExtend(*busLoad, 64);
		writeX(inst.rd, res);
		break;
	}

	case Op::LBU: {
		const auto addr = readX(inst.rs1) + inst.imm;
		const auto busLoad = bus.read(addr, 1);
		if (!busLoad)
			return Exception::LOAD_ACCESS_FAULT;
		
		writeX(inst.rd, *busLoad);
		break;
	}

	case Op::LHU: {
		const auto addr = readX(inst.rs1) + inst.imm;
		const auto busLoad = bus.read(addr, 2);
		if (!busLoad)
			return Exception::LOAD_ACCESS_FAULT;
		
		writeX(inst.rd, *busLoad);
		break;
	}

	case Op::LWU: {
		const auto addr = readX(inst.rs1) + inst.imm;
		const auto busLoad = bus.read(addr, 4);
		if (!busLoad)
			return Exception::LOAD_ACCESS_FAULT;
		
		writeX(inst.rd, *busLoad);
		break;
	}

	case Op::SB: {
		const auto addr = readX(inst.rs1) + inst.imm;
		const auto busLoad = bus.write(addr, 1, readX(inst.rs2));
		if (!busLoad)
			return Exception::STORE_ACCESS_FAULT;
		break;
	}

	case Op::SH: {
		const auto addr = readX(inst.rs1) + inst.imm;
		const auto busLoad = bus.write(addr, 2, readX(inst.rs2));
		if (!busLoad)
			return Exception::STORE_ACCESS_FAULT;
		break;
	}

	case Op::SW: {
		const auto addr = readX(inst.rs1) + inst.imm;
		const auto busLoad = bus.write(addr, 4, readX(inst.rs2));
		if (!busLoad)
			return Exception::STORE_ACCESS_FAULT;
		break;
	}

	case Op::SD: {
		const auto addr = readX(inst.rs1) + inst.imm;
		const auto busLoad = bus.write(addr, 8, readX(inst.rs2));
		if (!busLoad)
			return Exception::STORE_ACCESS_FAULT;
		break;
	}

	case Op::ADDI: {
		writeX(inst.rd, readX(inst.rs1) + inst.imm);
		break;
	}

	case Op::XORI: {
		writeX(inst.rd, readX(inst.rs1) ^ inst.imm);
		break;
	}

	case Op::ORI: {
		writeX(inst.rd, readX(inst.rs1) | inst.imm);
		break;
	}

	case Op::ANDI: {
		writeX(inst.rd, readX(inst.rs1) & inst.imm);
		break;
	}

	case Op::SLTIU: {
		writeX(inst.rd, readX(inst.rs1) < inst.imm);
		break;
	}

	case Op::SLTI: {
		const auto lhs = static_cast<std::int64_t>(readX(inst.rs1));
		const auto rhs = static_cast<std::int64_t>(inst.imm);
		writeX(inst.rd, lhs < rhs);
		break;
	}

	case Op::SLLI: {
		writeX(inst.rd, readX(inst.rs1) << inst.shiftAmt);
		break;
	}

	case Op::SRLI: {
		writeX(inst.rd, readX(inst.rs1) >> inst.shiftAmt);
		break;
	}

	case Op::SRAI: {
		writeX(inst.rd, signExtend(readX(inst.rs1) >> inst.shiftAmt, 
			64 - inst.shiftAmt));
		break;
	}

	case Op::ADD: {
		writeX(inst.rd, readX(inst.rs1) + readX(inst.rs2));
		break;
	}

	case Op::SUB: {
		writeX(inst.rd, readX(inst.rs1) - readX(inst.rs2));
		break;
	}

	case Op::SLL: {
		const auto shiftAmt = readX(inst.rs2) & 0b111111;
		writeX(inst.rd, readX(inst.rs1) << shiftAmt);
		break;
	}

	case Op::SLT: {
		auto const lhs = static_cast<std::int64_t>(readX(inst.rs1));
		auto const rhs = static_cast<std::int64_t>(readX(inst.rs2));
		writeX(inst.rd, lhs < rhs);
		break;
	}

	case Op::SLTU: {
		writeX(inst.rd, readX(inst.rs1) < readX(inst.rs2));
		break;
	}

	case Op::XOR: {
		writeX(inst.rd, readX(inst.rs1) ^ readX(inst.rs2));
		break;
	}

	case Op::SRL: {
		const auto shiftAmt = readX(inst.rs2) & 0b111111;
		writeX(inst.rd, readX(inst.rs1) >> shiftAmt);
		break;
	}

	case Op::SRA: {
		const auto shiftAmt = readX(inst.rs2) & 0b111111;
		writeX(inst.rd, signExtend(readX(inst.rs1) >> shiftAmt, 
			64 - shiftAmt));
		break;
	}

	case Op::OR: {
		writeX(inst.rd, readX(inst.rs1) | readX(inst.rs2));
		break;
	}

	case Op::AND: {
		writeX(inst.rd, readX(inst.rs1) & readX(inst.rs2));
		break;
	}

	case Op::FENCE: {
		// WORKAROUND: As the Cpu is syncronised so not required
		break;
	}

	case Op::FENCE_TSO: {
		// WORKAROUND: As the Cpu is syncronised so not required
		break;
	}

	case Op::PAUSE: {
		// WORKAROUND: As the Cpu is syncronised so not required
		break;
	}

	case Op::ECALL: {
		auto currentMode = readMode();
		if (currentMode == Privilege::USER)
			return Exception::ENVIRONMENT_CALL_FROM_UMODE;
		else if (currentMode == Privilege::SUPERVISOR)
			return Exception::ENVIRONMENT_CALL_FROM_SMODE;
		else if (currentMode == Privilege::MACHINE)
			return Exception::ENVIRONMENT_CALL_FROM_MMODE;
		return Exception::ILLEGAL_INSTRUCTION;
	}

	case Op::EBREAK: {
		return Exception::BREAKPOINT;
	}

	case Op::ADDIW: {
		const auto res = static_cast<std::uint32_t>(readX(inst.rs1) 
			+ inst.imm);
		writeX(inst.rd, signExtend(res, 32));
		break;
	}

	case Op::SLLIW: {
		const auto res = static_cast<std::uint32_t>(readX(inst.rs1) 
			<< inst.shiftAmt);
		writeX(inst.rd, signExtend(res, 32));
		break;
	}

	case Op::SRLIW: {
		const auto word = static_cast<std::uint32_t>(readX(inst.rs1));
		const auto res = word >> inst.shiftAmt;
		writeX(inst.rd, signExtend(res, 32));
		break;
	}

	case Op::SRAIW: {
		const auto word = static_cast<std::uint32_t>(readX(inst.rs1));
		const auto res = word >> inst.shiftAmt;
		writeX(inst.rd, signExtend(res, 32 - inst.shiftAmt));
		break;
	}

	case Op::ADDW: {
		const auto word = readX(inst.rs1) + readX(inst.rs2);
		const auto res = static_cast<std::uint32_t>(word);
		writeX(inst.rd, signExtend(res, 32));
		break;
	}

	case Op::SUBW: {
		const auto word = readX(inst.rs1) - readX(inst.rs2);
		const auto res = static_cast<std::uint32_t>(word);
		writeX(inst.rd, signExtend(res, 32));
		break;
	}

	case Op::SLLW: {
		const auto shift = readX(inst.rs2) & 0b11111;
		const auto word = readX(inst.rs1) << shift;
		const auto res = static_cast<std::uint32_t>(word);
		writeX(inst.rd, signExtend(res, 32));
		break;
	}

	case Op::SRLW: {
		const auto shift = readX(inst.rs2) & 0b11111;
		const auto word = static_cast<std::uint32_t>(readX(inst.rs1));
		const auto res = word >> shift;
		writeX(inst.rd, signExtend(res, 32));
		break;
	}

	case Op::SRAW: {
		const auto shift = readX(inst.rs2) & 0b11111;
		const auto word = static_cast<std::uint32_t>(readX(inst.rs1));
		const auto res = word >> shift;
		writeX(inst.rd, signExtend(res, 32 - shift));
		break;
	}

	case Op::MUL: {
		writeX(inst.rd, readX(inst.rs1) * readX(inst.rs2));
		break;
	}

	case Op::MULH: {
		auto lhs = readX(inst.rs1);
		auto rhs = readX(inst.rs2);

		auto res = multHighUnsigned(lhs, rhs);
		if (lhs >> 63) res -= rhs;
		if (rhs >> 63) res -= lhs;

		writeX(inst.rd, res);
		break;
	}

	case Op::MULHSU: {
		const auto lhs = readX(inst.rs1);
		const auto rhs = readX(inst.rs2);

		auto res = multHighUnsigned(lhs, rhs);
		if (lhs >> 63) res -= rhs;

		writeX(inst.rd, res);
		break;
	}

	case Op::MULHU: {
		const auto lhs = readX(inst.rs1);
		const auto rhs = readX(inst.rs2);

		const auto res = multHighUnsigned(lhs, rhs);

		writeX(inst.rd, res);
		break;
	}

	case Op::DIV: {
		const auto lhs = static_cast<std::int64_t>(readX(inst.rs1));
		const auto rhs = static_cast<std::int64_t>(readX(inst.rs2));

		const auto min = std::numeric_limits<std::int64_t>::min();

		std::uint64_t res = 0;
		if (rhs == 0) res = -1;
		else if (lhs == min && rhs == -1) res = min;
		else res = lhs / rhs;

		writeX(inst.rd, res);
		break;
	}

	case Op::DIVU: {
		const auto lhs = readX(inst.rs1);
		const auto rhs = readX(inst.rs2);

		std::uint64_t res = 0;
		if (rhs == 0) res = std::numeric_limits<std::uint64_t>::max();
		else res = lhs / rhs;

		writeX(inst.rd, res);
		break;
	}

	case Op::REM: {
		const auto lhs = static_cast<std::int64_t>(readX(inst.rs1));
		const auto rhs = static_cast<std::int64_t>(readX(inst.rs2));

		const auto min = std::numeric_limits<std::int64_t>::min();

		std::int64_t res = 0;
		if (rhs == 0) res = lhs;
		else if (lhs == min && rhs == -1) res = 0;
		else res = lhs % rhs;

		writeX(inst.rd, res);
		break;
	}

	case Op::REMU: {
		const auto lhs = readX(inst.rs1);
		const auto rhs = readX(inst.rs2);

		std::uint64_t res = 0;
		if (rhs == 0) res = lhs;
		else res = lhs % rhs;

		writeX(inst.rd, res);
		break;
	}

	case Op::MULW: {
		const auto lhs = static_cast<std::int32_t>(readX(inst.rs1));
		const auto rhs = static_cast<std::int32_t>(readX(inst.rs2));

		const std::uint32_t res = lhs * std::int64_t{1} * rhs;
		writeX(inst.rd, signExtend(res, 32));
		break;
	}

	case Op::DIVW: {
		const auto lhs = static_cast<std::int32_t>(readX(inst.rs1));
		const auto rhs = static_cast<std::int32_t>(readX(inst.rs2));

		const auto min = std::numeric_limits<std::int32_t>::min();

		std::int64_t res = 0;
		if (rhs == 0) res = -1;
		else if (lhs == min && rhs == -1) res = min;
		else res = lhs * std::int64_t{1} / rhs;

		writeX(inst.rd, res);
		break;
	}

	case Op::DIVUW: {
		const auto lhs = static_cast<std::uint32_t>(readX(inst.rs1));
		const auto rhs = static_cast<std::uint32_t>(readX(inst.rs2));

		std::int64_t res = 0;
		if (rhs == 0) res = std::numeric_limits<std::uint32_t>::max();
		else res = lhs * std::int64_t{1} / rhs;

		writeX(inst.rd, signExtend(res, 32));
		break;
	}

	case Op::REMW: {
		const auto lhs = static_cast<std::int32_t>(readX(inst.rs1));
		const auto rhs = static_cast<std::int32_t>(readX(inst.rs2));

		const auto min = std::numeric_limits<std::int32_t>::min();

		std::int64_t res = 0;
		if (rhs == 0) res = lhs;
		else if (lhs == min && rhs == -1) res = 0;
		else res = lhs % rhs;

		writeX(inst.rd, res);
		break;
	}

	case Op::REMUW: {
		const auto lhs = static_cast<std::uint32_t>(readX(inst.rs1));
		const auto rhs = static_cast<std::uint32_t>(readX(inst.rs2));

		std::uint64_t res = 0;
		if (rhs == 0) res = lhs;
		else res = lhs % rhs;

		writeX(inst.rd, signExtend(static_cast<std::uint32_t>(res), 32));
		break;
	}

	case Op::CSRRW: {
		const auto error = validateCsr(inst.csr, true);
		if (error) return *error;

		const auto source = readX(inst.rs1);
		const auto oldCsr = inst.rd != 0 
			? readCsrInternal(inst.csr) : 0;
	
		const auto res = writeCsr(inst.csr, source);
		if (!res) return res.error();

		writeX(inst.rd, oldCsr);
		break;
	}

	case Op::CSRRS: {
		// Page 66 - Last Paragraph
		// For both CSRRS and CSRRC, if rs1=x0, then the instruction 
		// will not write to the CSR at all, and so shall not
		// cause any of the side effects that might otherwise occur 
		// on a CSR write, nor raise illegal-instruction exceptions on 
		// accesses to read-only CSRs.
		const auto writes = inst.rs1 != 0;
	
		const auto error = validateCsr(inst.csr, writes);
		if (error) return *error;

		const auto source = readX(inst.rs1);
		const auto oldCsr = readCsrInternal(inst.csr);

		if (writes) {
			const auto res = writeCsr(inst.csr, oldCsr | source);
			if (!res) return res.error();
		}

		writeX(inst.rd, oldCsr);
		break;
	}

	case Op::CSRRC: {
		// Page 66 - Last Paragraph
		// For both CSRRS and CSRRC, if rs1=x0, then the instruction 
		// will not write to the CSR at all, and so shall not
		// cause any of the side effects that might otherwise occur 
		// on a CSR write, nor raise illegal-instruction exceptions on 
		// accesses to read-only CSRs.
		const auto writes = inst.rs1 != 0;
	
		const auto error = validateCsr(inst.csr, writes);
		if (error) return *error;

		const auto source = readX(inst.rs1);
		const auto oldCsr = readCsrInternal(inst.csr);

		if (writes) {
			const auto res = writeCsr(inst.csr, oldCsr & ~source);
			if (!res) return res.error();
		}

		writeX(inst.rd, oldCsr);
		break;
	}

	case Op::CSRRWI: {
		const auto error = validateCsr(inst.csr, true);
		if (error) return *error;

		const auto source = inst.rs1;
		const auto oldCsr = inst.rd != 0 
			? readCsrInternal(inst.csr) : 0;

		const auto res = writeCsr(inst.csr, source);
		if (!res) return res.error();

		writeX(inst.rd, oldCsr);
		break;
	}

	case Op::CSRRSI: {
		const auto writes = inst.rs1 != 0;
	
		const auto error = validateCsr(inst.csr, writes);
		if (error) return *error;

		const auto source = inst.rs1;
		const auto oldCsr = readCsrInternal(inst.csr);

		if (writes) {
			const auto res = writeCsr(inst.csr, oldCsr | source);
			if (!res) return res.error();
		}

		writeX(inst.rd, oldCsr);
		break;
	}

	case Op::CSRRCI: {
		const auto writes = inst.rs1 != 0;
	
		const auto error = validateCsr(inst.csr, writes);
		if (error) return *error;

		const std::uint64_t source = inst.rs1;
		const auto oldCsr = readCsrInternal(inst.csr);

		if (writes) {
			const auto res = writeCsr(inst.csr, oldCsr & ~source);
			if (!res) return res.error();
		}

		writeX(inst.rd, oldCsr);
		break;
	}

	case Op::LR_W: {
		const auto addr = readX(inst.rs1);
		if (addr % 4 != 0)
			return Exception::LOAD_ADDRESS_MISALIGNED;

		const auto busLoad = bus.read(addr, 4);
		if (!busLoad)
			return Exception::LOAD_ACCESS_FAULT;

		vReservedMemory = ReservedMemory{
			.addr  = addr,
			.width = 4,
		};

		auto res = signExtend(*busLoad, 32);
		writeX(inst.rd, res);
		break;
	}

	case Op::SC_W: {
		const auto addr = readX(inst.rs1);
		const auto value = readX(inst.rs2);
		const bool matches = vReservedMemory
			&& vReservedMemory->addr == addr
			&& vReservedMemory->width == 4;

		vReservedMemory = std::nullopt;

		if (addr % 4 != 0)
			return Exception::STORE_ADDRESS_MISALIGNED;

		if (!bus.writeCheck(addr, 4))
			return Exception::STORE_ACCESS_FAULT;

		if (!matches) {
			writeX(inst.rd, 1);
			break;
		}

		const auto busLoad = bus.write(addr, 4, value);
		if (!busLoad)
			return Exception::STORE_ACCESS_FAULT;

		writeX(inst.rd, 0);
		break;
	}

	case Op::AMOSWAP_W: {
		const auto addr = readX(inst.rs1);
		if (addr % 4 != 0)
			return Exception::STORE_ADDRESS_MISALIGNED;

		const std::uint32_t source = readX(inst.rs2);
		const auto old = bus.read(addr, 4);
		if (!old) 
			return Exception::STORE_ACCESS_FAULT;

		const auto res = source;
		auto writeSuccess = bus.write(addr, 4, res);
		if (!writeSuccess)
			return Exception::STORE_ACCESS_FAULT;

		writeX(inst.rd, signExtend(*old, 32));
		break;
	}

	case Op::AMOADD_W: {
		const auto addr = readX(inst.rs1);
		if (addr % 4 != 0)
			return Exception::STORE_ADDRESS_MISALIGNED;

		const std::uint32_t source = readX(inst.rs2);
		const auto old = bus.read(addr, 4);
		if (!old) 
			return Exception::STORE_ACCESS_FAULT;

		const auto res = *old + source;
		auto writeSuccess = bus.write(addr, 4, res);
		if (!writeSuccess)
			return Exception::STORE_ACCESS_FAULT;

		writeX(inst.rd, signExtend(*old, 32));
		break;
	}

	case Op::AMOXOR_W: {
		const auto addr = readX(inst.rs1);
		if (addr % 4 != 0)
			return Exception::STORE_ADDRESS_MISALIGNED;

		const std::uint32_t source = readX(inst.rs2);
		const auto old = bus.read(addr, 4);
		if (!old) 
			return Exception::STORE_ACCESS_FAULT;

		const auto res = *old ^ source;
		auto writeSuccess = bus.write(addr, 4, res);
		if (!writeSuccess)
			return Exception::STORE_ACCESS_FAULT;

		writeX(inst.rd, signExtend(*old, 32));
		break;
	}

	case Op::AMOAND_W: {
		const auto addr = readX(inst.rs1);
		if (addr % 4 != 0)
			return Exception::STORE_ADDRESS_MISALIGNED;

		const std::uint32_t source = readX(inst.rs2);
		const auto old = bus.read(addr, 4);
		if (!old) 
			return Exception::STORE_ACCESS_FAULT;

		const auto res = *old & source;
		auto writeSuccess = bus.write(addr, 4, res);
		if (!writeSuccess)
			return Exception::STORE_ACCESS_FAULT;

		writeX(inst.rd, signExtend(*old, 32));
		break;
	}

	case Op::AMOOR_W: {
		const auto addr = readX(inst.rs1);
		if (addr % 4 != 0)
			return Exception::STORE_ADDRESS_MISALIGNED;

		const std::uint32_t source = readX(inst.rs2);
		const auto old = bus.read(addr, 4);
		if (!old) 
			return Exception::STORE_ACCESS_FAULT;

		const auto res = *old | source;
		auto writeSuccess = bus.write(addr, 4, res);
		if (!writeSuccess)
			return Exception::STORE_ACCESS_FAULT;

		writeX(inst.rd, signExtend(*old, 32));
		break;
	}

	case Op::AMOMIN_W: {
		const auto addr = readX(inst.rs1);
		if (addr % 4 != 0)
			return Exception::STORE_ADDRESS_MISALIGNED;

		const std::int32_t source = readX(inst.rs2);
		const auto old = bus.read(addr, 4);
		if (!old) 
			return Exception::STORE_ACCESS_FAULT;

		const auto res = std::min(static_cast<std::int32_t>(*old), 
			source);
		auto writeSuccess = bus.write(addr, 4, res);
		if (!writeSuccess)
			return Exception::STORE_ACCESS_FAULT;

		writeX(inst.rd, signExtend(*old, 32));
		break;
	}

	case Op::AMOMAX_W: {
		const auto addr = readX(inst.rs1);
		if (addr % 4 != 0)
			return Exception::STORE_ADDRESS_MISALIGNED;

		const std::int32_t source = readX(inst.rs2);
		const auto old = bus.read(addr, 4);
		if (!old) 
			return Exception::STORE_ACCESS_FAULT;

		const auto res = std::max(static_cast<std::int32_t>(*old), 
			source);
		auto writeSuccess = bus.write(addr, 4, res);
		if (!writeSuccess)
			return Exception::STORE_ACCESS_FAULT;

		writeX(inst.rd, signExtend(*old, 32));
		break;
	}

	case Op::AMOMINU_W: {
		const auto addr = readX(inst.rs1);
		if (addr % 4 != 0)
			return Exception::STORE_ADDRESS_MISALIGNED;

		const std::uint32_t source = readX(inst.rs2);
		const auto old = bus.read(addr, 4);
		if (!old) 
			return Exception::STORE_ACCESS_FAULT;

		const auto res = std::min(static_cast<std::uint32_t>(*old), 
			source);
		auto writeSuccess = bus.write(addr, 4, res);
		if (!writeSuccess)
			return Exception::STORE_ACCESS_FAULT;

		writeX(inst.rd, signExtend(*old, 32));
		break;
	}

	case Op::AMOMAXU_W: {
		const auto addr = readX(inst.rs1);
		if (addr % 4 != 0)
			return Exception::STORE_ADDRESS_MISALIGNED;

		const std::uint32_t source = readX(inst.rs2);
		const auto old = bus.read(addr, 4);
		if (!old) 
			return Exception::STORE_ACCESS_FAULT;

		const auto res = std::max(static_cast<std::uint32_t>(*old), 
			source);
		auto writeSuccess = bus.write(addr, 4, res);
		if (!writeSuccess)
			return Exception::STORE_ACCESS_FAULT;

		writeX(inst.rd, signExtend(*old, 32));
		break;
	}

	default:
		return Exception::ILLEGAL_INSTRUCTION;
	}

	writePc(nextPc);
	return std::nullopt;
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

constexpr std::uint32_t decodeCsr(std::uint32_t raw) {
	return (raw >> 20) & 0b111111111111;
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

constexpr std::uint64_t multHighUnsigned(std::uint64_t a, std::uint64_t b) {
	const auto a0 = a & 0xffffffff;
	const auto a1 = a >> 32;
	const auto b0 = b & 0xffffffff;
	const auto b1 = b >> 32;

	const auto p00 = a0 * b0;
	const auto p10 = a1 * b0;
	const auto p01 = a0 * b1;
	const auto p11 = a1 * b1;

	const auto carry = (p00 >> 32) + (p01 & 0xffffffff) 
		+ (p10 & 0xffffffff);
	
	return p11 + (p01 >> 32) + (p10 >> 32) + (carry >> 32);
}

}

