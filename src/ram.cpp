#include "ram.hpp"
#include "util.hpp"

namespace rv64emu {

Ram::Ram(std::uint64_t base, std::size_t bytes) {
	if (!valid_memory_region(base, bytes)) {
		throw std::invalid_argument(
			"RAM should be nonempty and fit in physical memory");
	}
	vBase = base;
	vBytes.resize(bytes, 0);
}

bool Ram::contains(std::uint64_t addr, std::size_t width) {
	return contains_range(base(), size(), addr, width);
}

std::uint64_t Ram::base() const {
	return vBase;
}

std::size_t Ram::size() const {
	return vBytes.size();
}

std::optional<std::uint64_t> Ram::read(std::uint64_t addr, std::size_t width) {
	if (!valid_memory_width(width) || !contains(addr, width)) {
		return std::nullopt;
	}
	const auto offset = static_cast<std::size_t>(addr - vBase);
	const auto subspan = 
		std::span<std::uint8_t>(vBytes).subspan(offset, width);
	return load_little_endian(subspan);
}

bool Ram::write(std::uint64_t addr, std::size_t width, std::uint64_t value) {
	if (!valid_memory_width(width) || !contains(addr, width)) {
		return false;
	}
	const auto offset = static_cast<std::size_t>(addr - vBase);
	const auto subspan = 
		std::span<std::uint8_t>(vBytes).subspan(offset, width);
	store_little_endian(subspan, value);
	return true;
}

};

