#include "util.hpp"

namespace rv64emu {

bool validMemoryRegion(std::uint64_t base, std::size_t size) {
	return size != 0 
		&& size - 1 <= std::numeric_limits<std::uint64_t>::max() - base;
}

bool validMemoryWidth(std::size_t width) {
	return width == 1 || width == 2 || width == 4 || width == 8;
}

bool containsRange(std::uint64_t base, std::size_t size, std::uint64_t addr,
	std::size_t width) {
	return addr >= base && width != 0 && width <= size
		&& addr - base <= size - width;
}

std::uint64_t loadLittleEndian(std::span<std::uint8_t> bytes) {
	std::uint64_t res = 0;
	for (std::size_t i = 0; i < bytes.size(); i++) {
		res |= static_cast<std::uint64_t>(bytes[i]) << (i * 8);
	}
	return res;
}

void storeLittleEndian(std::span<std::uint8_t> bytes, std::uint64_t value) {
	for (std::size_t i = 0; i < bytes.size(); i++) {
		bytes[i] = static_cast<std::uint8_t>(value >> (i * 8));
	}
}

}

