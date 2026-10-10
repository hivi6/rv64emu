#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdio>

#include "cpu.hpp"
#include "ram.hpp"
#include "uart.hpp"

using namespace std;
using namespace rv64emu;

vector<uint8_t> loadBin(string filepath) {
	ifstream file(filepath, ios::binary | ios::ate);
	if (!file.is_open()) {
		cerr << "Error opening binary file!" << endl;
		exit(1);
	}

	streamsize size = file.tellg();
	file.seekg(0, ios::beg);

	vector<uint8_t> buffer(size);
	if (!file.read((char*)buffer.data(), size)) {
		cerr << "Error reading file" << endl;
		exit(1);
	}

	return buffer;
}

int main(int argc, char **argv) {
	if (argc <= 1) {
		cerr << "Expected filepath to binary" << endl;
		return 1;
	}

	string filepath(argv[1]);
	Ram ram(0x1000, loadBin(filepath));

	Uart uart(0x0, 1, 
	[]() -> std::uint8_t {
		return static_cast<char>(getchar());
	}, 
	[](std::uint8_t byte) {
		cout << static_cast<char>(byte) << flush;
	});

	Bus bus;
	bus.attach(ram);
	bus.attach(uart);

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

