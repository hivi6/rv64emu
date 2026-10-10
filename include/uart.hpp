#pragma once

#include "bus_device.hpp"

namespace rv64emu {

class Uart : public BusDevice {
	using InputCallback = std::function<std::optional<std::uint8_t>()>;
	using OutputCallback = std::function<void(std::uint8_t)>;

	std::uint64_t  vBase;
	std::size_t    vSize;
	InputCallback  vInputCallback;  // what to do when read is called
	OutputCallback vOutputCallback; // what to do when write is called

public:
	/**
	 * Constructor for the UART device
	 *
	 * Parameters:
	 * 	base   = base of the device where it will get attached to bus
	 *      size   = size of the device where it will get attached to bus
	 *      input  = callback function when read function is called
	 *      output = callback function when write function is called
	 */
	Uart(std::uint64_t base, std::size_t size, 
		InputCallback input, OutputCallback output);

	/**
	 * Get the base of the UART device
	 *
	 * Returns:
	 * 	uint64_t = base of the ram device
	 */
	std::uint64_t base() const override;

	/**
	 * Get the size of the UART device
	 *
	 * Returns:
	 * 	size_t = size of the ram device
	 */
	std::size_t size() const override;

	/**
	 * Run the input callback and return the output of the callback
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
	 * Write the output callback and return if the write worked
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

