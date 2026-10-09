#pragma once

#include <array>

namespace rv64emu {

class Cpu {
	std::array<uint64_t, 32> x;  // Register for the cpu
	uint64_t                 pc; // Program counter

public:
	Cpu();
	
	// Reading registers
	uint64_t readPC() const;
	uint64_t readX(size_t index) const;

	// Writing registers
	void writePC(uint64_t value);
	void writeX(size_t index, uint64_t value);
};

};

