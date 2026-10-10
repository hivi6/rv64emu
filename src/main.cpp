#include <iostream>

#include "cpu.hpp"
#include "ram.hpp"
#include "uart.hpp"

using namespace std;
using namespace rv64emu;

int main() {
	Ram ram(0x1000, 1024);

	Uart displayUart(0x0, 1, {}, [](std::uint8_t byte) {
		cout << static_cast<char>(byte) << flush;
	});

	Bus bus;
	bus.attach(ram);
	bus.attach(displayUart);

	// load a hello world program
	auto addr = ram.base();
	for (auto ch: string("Hello, World!\n")) {
		// addi x1, x0, char
		auto addi = (static_cast<std::uint32_t>(ch) << 20) | 0x00000093;
		bus.write(addr, 4, addi);
		addr += 4;
		
		// sb x1, 0(0x0); write character to UART address 0
		bus.write(addr, 4, 0x00100023);
		addr += 4;
	}
	// ebreak
	bus.write(addr, 4, 0x00100073);

	Cpu cpu;
	cpu.writePc(ram.base());

	for (size_t step = 1; step < 1000; step++) {
		auto raw = cpu.fetch(bus);
		if (!raw) break;

		auto inst = cpu.decode(*raw);
		if (!inst) break;

		auto exception = cpu.execute(*inst, bus);
		if (exception) break;
	}

	return 0;
}

