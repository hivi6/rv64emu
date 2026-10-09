#pragma once

#include "common.hpp"

namespace rv64emu {

/**
 * Check if a given base and size is a valid memory region
 *
 * Parameters:
 * 	base = base of the device
 * 	size = size of the device
 *
 * Returns:
 * 	bool = if the memory region is valid or not
 */
bool valid_memory_region(std::uint64_t base, std::size_t size);

/**
 * Check if a given width is valid
 *
 * Parameters:
 * 	width = width of a memory access
 *
 * Returns:
 * 	bool = if the memory width is valid
 */
bool valid_memory_width(std::size_t width);

/**
 * Check if the following address with a given width belongs in
 * a device with a given base and size
 *
 * Parameters:
 * 	base  = base of the device
 *	size  = size of the device
 *	addr  = address in the device
 *	width = width of the value that will be accessed in the given address
 *
 * Returns:
 * 	bool = if the parameters are valid
 */
bool contains_range(std::uint64_t base, std::size_t size, std::uint64_t addr,
	std::size_t width);

/**
 * Load little endian from a given span of bytes
 *
 * Parameters:
 * 	bytes = span of bytes
 *
 * Returns:
 * 	uint64_t = little endian value
 */
std::uint64_t load_little_endian(std::span<std::uint8_t> bytes);

/**
 * Store little endian value into a given span
 *
 * Parameters:
 * 	bytes = span where value is inserted
 * 	value = value that is inserted
 */
void store_little_endian(std::span<std::uint8_t> bytes, std::uint64_t value);

};

