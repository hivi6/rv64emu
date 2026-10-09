#pragma once

#include "common.hpp"

namespace rv64emu {

class BusDevice {
public:
	virtual ~BusDevice() = default;

	/**
	 * Get the base pointer where the device attaches to the bus
	 *
	 * Returns:
	 * 	uint64_t = base pointer where it attaches to the bus
	 */
	virtual std::uint64_t base() const = 0;

	/**
	 * Returns the size consumed by the device on the bus
	 *
	 * Returns:
	 * 	size_t = size of the device consumed on the bus
	 */
	virtual std::size_t size() const = 0;

	/**
	 * Read content at the given address of a given width
	 * width = {1, 2, 4, 8} byte
	 *
	 * Parameters:
	 * 	addr  = address from where reading needs to be done
	 *      width = how much width needs to be read
	 *
	 * Returns:
	 * 	uint64_t = if read success
	 *      nullopt  = if read fails
	 */
	virtual std::optional<std::uint64_t> read(std::uint64_t addr, 
		std::size_t width) = 0;

	/**
	 * Write value to a given address of a given width
	 *
	 * Parameters:
	 * 	addr  = address where writing needs to be done
	 * 	width = how much width needs to be write
	 * 	value = What value needs to be written
	 */
	virtual bool write(std::uint64_t addr, std::size_t width, 
		std::uint64_t value) = 0;
};

};

