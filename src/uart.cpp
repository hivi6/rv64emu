#include "uart.hpp"

namespace rv64emu {

Uart::Uart(std::uint64_t base, std::size_t size, 
	InputCallback input, OutputCallback output) {
	vBase = base;
	vSize = size;
	vInputCallback = input;
	vOutputCallback = output;
}

std::uint64_t Uart::base() const {
	return vBase;
}

std::size_t Uart::size() const {
	return vSize;
};

std::optional<std::uint64_t> Uart::read(std::uint64_t addr, std::size_t width) {
	if (addr == base() && width == 1) {
		auto res = vInputCallback();
		if (res) return static_cast<std::uint64_t>(res.value());
	}
	return std::nullopt;
}

bool Uart::write(std::uint64_t addr, std::size_t width, std::uint64_t value) {
	if (addr == base() && width == 1) {
		vOutputCallback(static_cast<std::uint8_t>(value));
		return true;
	}
	return false;
}

};

