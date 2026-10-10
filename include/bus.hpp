#pragma once

#include "common.hpp"
#include "bus_device.hpp"

namespace rv64emu {

class Bus {
	std::vector<BusDevice*> vDevices; // All attached devices

public:
	/**
	 * Attach a given device to the bus
	 *
	 * Parameters:
	 * 	device = bus device that will be attached to the bus
	 *
	 * Returns:
	 * 	bool = true if successful else false
	 */
	bool attach(BusDevice &device);

	/**
	 * Read address of a given width from the given bus
	 *
	 * Parameters:
	 * 	addr  = address
	 *	width = width of the value
	 *
	 * Returns:
	 * 	uint64_t = if read was successful
	 *	nullopt  = if read was failure
	 */
	std::optional<std::uint64_t> read(std::uint64_t addr, 
		std::size_t width);

	/**
	 * Write value to a given address of a given width to the given bus
	 *
	 * Parameters:
	 * 	addr  = address
	 *	width = width of the value
	 * 	value = value of that will be written
	 *
	 * Returns:
	 * 	true  = if read was successful
	 *	false = if read was failure
	 */
	bool write(std::uint64_t addr, std::size_t width, 
		std::uint64_t value);
};

};

