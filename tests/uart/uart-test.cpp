#include "cpu.hpp"
#include "ram.hpp"
#include "uart.hpp"

#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdio>

using namespace std;
using namespace rv64emu;

vector<uint8_t> loadBin(string filepath) {
	ifstream file(filepath, ios::binary | ios::ate);
	if (!file.is_open()) {
		cerr << "Error opening binary file!" << endl;
		return {};
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
	auto bin = loadBin(filepath);

	Ram ram(0x1000, bin.size());
	for (size_t i = 0; i < bin.size(); i++) {
		ram.write(ram.base() + i, 1, bin[i]);
	}

	Uart uart(0x0, 1, 
	[]() -> optional<uint8_t> {
		const int ch = std::getchar();
		if (ch == EOF) return std::nullopt;
		return static_cast<uint8_t>(ch);
	}, 
	[](uint8_t byte) {
		cout << static_cast<char>(byte) << flush;
	});

	Bus bus;
	bus.attach(ram);
	bus.attach(uart);

	Cpu cpu;
	cpu.writePc(ram.base());

	while (true) {
		auto rawInst = cpu.fetch(bus);
		if (!rawInst) break;

		auto inst = cpu.decode(*rawInst);
		if (!inst) break;

		auto exception = cpu.execute(*inst, bus);
		if (exception) break;
	}

	return 0;
}


