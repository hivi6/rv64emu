#include "bus.hpp"
#include "util.hpp"

namespace rv64emu {

BusDevice *find(const std::vector<BusDevice*>& devices, 
	std::uint64_t addr, std::size_t width) {
	for (auto device: devices) {
		if (containsRange(device->base(), device->size(), addr, width))
			return device;
	}
	return nullptr;
}

bool Bus::attach(BusDevice &device) {
	const auto base = device.base();
	const auto size = device.size();

	if (!validMemoryRegion(base, size)) {
		return false;
	}

	for (auto attached: vDevices) {
		const auto attachedBase = attached->base();
		const auto attachedSize = attached->size();

		/*
		const bool isOverlapping = base <= attachedBase
			? base + size > attachedBase
			: attachedBase + attachedSize > base;
		// Handle integer overflow
		*/
		const bool isOverlapping = base <= attachedBase
			? size > attachedBase - base
			: attachedSize > base - attachedBase;
		
		
		if (isOverlapping) {
			return false;
		}
	}

	vDevices.push_back(&device);
	return true;
}

std::optional<std::uint64_t> Bus::read(std::uint64_t addr, 
	std::size_t width) {
	auto device = find(vDevices, addr, width);
	if (device == nullptr) {
		return std::nullopt;
	}
	return device->read(addr, width);
}

bool Bus::write(std::uint64_t addr, std::size_t width, std::uint64_t value) {
	auto device = find(vDevices, addr, width);
	if (device == nullptr) {
		return false;
	}
	return device->write(addr, width, value);
}

};
