#pragma once

#include <array>

namespace rv64emu {

class Cpu {
	std::array<uint64_t, 32> x;  // Register for the cpu
	uint64_t                 pc; // Program counter

public:
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
	uint64_t readPC() const;

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
	void writePC(uint64_t value);

	/**
	 * Change the register index value
	 * Trying to write register 0 will not cause any write
	 *
	 * Parameters:
	 * 	index = register index
	 * 	value = new value for the register
	 */
	void writeX(size_t index, uint64_t value);
};

};

