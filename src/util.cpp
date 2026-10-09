#include "util.hpp"

namespace rv64emu {

bool valid_memory_region(std::uint64_t base, std::size_t size) {
	return size != 0 
		&& size - 1 <= std::numeric_limits<std::uint64_t>::max() - base;
}

bool valid_memory_width(std::size_t width) {
	return width == 1 || width == 2 || width == 4 || width == 8;
}

bool contains_range(std::uint64_t base, std::size_t size, std::uint64_t addr,
	std::size_t width) {
	return addr >= base && width != 0 && width <= size
		&& addr - base <= size - width;
}

std::uint64_t load_little_endian(std::span<std::uint8_t> bytes) {
	std::uint64_t res = 0;
	for (std::size_t i = 0; i < bytes.size(); i++) {
		res |= static_cast<std::uint64_t>(bytes[i]) << (i * 8);
	}
	return res;
}

void store_little_endian(std::span<std::uint8_t> bytes, std::uint64_t value) {
	for (std::size_t i = 0; i < bytes.size(); i++) {
		bytes[i] = static_cast<std::uint8_t>(value >> (i * 8));
	}
}

}

