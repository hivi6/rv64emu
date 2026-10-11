#include "cpu.hpp"
#include "ram.hpp"

#include <iostream>
#include <fstream>
#include <sstream>

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

string toHex(uint64_t value) {
	ostringstream out;

	out << "0x"
	    << hex
	    << setw(sizeof(uint64_t) * 2)
	    << setfill('0')
	    << value;

	return out.str();
}

void printRegisters(const Cpu &cpu) {
        cout << "pc : " << toHex(cpu.readPc()) << endl;
        for (int i = 0; i < 8; i++) {
                for (int j = 0; j < 4; j++) {
                        int reg = i * 4 + j;
                        auto regStr = to_string(reg);
                        if (regStr.size() <= 1) regStr.push_back(' ');
                        cout << "x" << regStr << " : "
                                << toHex(cpu.readX(reg)) << " ";
                }
                cout << endl;
        }
}

int main(int argc, char **argv) {
	if (argc <= 1) {
		cerr << "Expected filepath to binary" << endl;
		return 1;
	}

	string filepath(argv[1]);
	auto bin = loadBin(filepath);

	Ram ram(0, bin.size());
	for (size_t i = 0; i < bin.size(); i++) {
		ram.write(i, 1, bin[i]);
	}

	Bus bus;
	bus.attach(ram);

	Cpu cpu;

	for (int step = 1; step < 100000; step++) {
		auto rawInst = cpu.fetch(bus);
		if (!rawInst) break;

		std::cout << "STEP: " << step << std::endl;

		auto inst = cpu.decode(*rawInst);
		if (!inst) break;

		auto exception = cpu.execute(*inst, bus);

		printRegisters(cpu);
		cout << endl;

		if (exception) break;
	}

	return 0;
}

