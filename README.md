# rv64emu

A RISCV 64-bit emulator

## Pre-requisites

In mac download the following package using brew:

```sh
brew install riscv64-elf-gcc riscv64-elf-binutils
```

## Testing

Now you can test using the following commands:

```
make test
```

## TODO

- [ ] Unpriv instructions
	- [x] I - Extension
	- [x] M - Extension
	- [x] Zicsr - Extension
	- [x] A - Extension
	- [ ] C - Extension
	- [ ] D - Extension
- [ ] Priv instructions
	- [ ] Csr permission

## Resources

- https://book.rvemu.app/hardware-components/01-cpu.html
- https://docs.riscv.org/reference/isa/_attachments/riscv-unprivileged.pdf
	- "Chapter 36. RV32/64G Instruction Set Listings" of the above pdf
	- "Chapter 2. RV32I Base Integer Instruction Set"
	- "Chapter 12. "M" Extension for Integer Multiplication and Division"
- https://docs.riscv.org/reference/isa/_attachments/riscv-privileged.pdf
	- "Chapter 19. RISC-V Privileged Instruction Set Listings"
	- Table 50 - For list of Exception

