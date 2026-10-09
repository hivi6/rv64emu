#pragma once

#include "bus_device.hpp"

namespace rv64emu {

class Ram : public BusDevice {
	std::uint64_t             vBase;  // Store the base of the device
	std::vector<std::uint8_t> vBytes; // Store all the bytes of the ram

public:
	/**
	 * Constructor for the ram device
	 *
	 * Parameters:
	 * 	base  = base of the device where it will get attached to bus
	 * 	bytes = Size of the ram (in bytes)
	 */
	Ram(std::uint64_t base, std::size_t bytes);

	/**
	 * Check if the address of the given width contains in the ram
	 *
	 * Parameters:
	 * 	addr  = address in the ram
	 * 	width = width of the value
	 *
	 * Returns:
	 * 	bool = if the address with the width belongs in the ram
	 */
	bool contains(std::uint64_t addr, std::size_t width);

	/**
	 * Get the base of the ram device
	 *
	 * Returns:
	 * 	uint64_t = base of the ram device
	 */
	std::uint64_t base() const override;

	/**
	 * Get the size of the ram device
	 *
	 * Returns:
	 * 	size_t = size of the ram device
	 */
	std::size_t size() const override;

	/**
	 * Read content at the given address of a given width from the ram
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
	std::optional<std::uint64_t> read(std::uint64_t addr, 
		std::size_t width) override;

	/**
	 * Write value to a given address of a given width to the ram
	 *
	 * Parameters:
	 * 	addr  = address where writing needs to be done
	 * 	width = how much width needs to be write
	 * 	value = What value needs to be written
	 */
	bool write(std::uint64_t addr, std::size_t width, 
		std::uint64_t value) override;
};

};

