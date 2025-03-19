#include "CPU6502.hpp"
#include "C64/C64.hpp"

CPU6502::CPU6502(C64 *c64) : Component("CPU"),
							 mC64(c64),
							 A(0),
							 X(0), Y(0),
							 PS(0),
							 SP(0),
							 PC(0),
							 log(spdlog::get("CPU")) {
	assert(OpCodes.size() == 256);
	Reset();

	(this->*(OpCodes[0x01].InstFunc))();
	(this->*(OpCodes[0x02].InstFunc))();

	// auto f = (this->*OpCodes[3].InstFunc)();
}

CPU6502::~CPU6502() {
}

CPU6502::Instruction CPU6502::GetNextInstruction() {
	return OpCodes[mC64->load8(PC)];
}

void CPU6502::RunOneInstruction() {
	mCurrentInstruction = GetNextInstruction();
	(this->*(mCurrentInstruction.InstFunc))();
	PC += mCurrentInstruction.length;
}

u8 CPU6502::IOLoad8(u16 /* address */) const {
	return 0;
}

u16 CPU6502::IOLoad16(u16 /* address */) const {
	return 0;
}

void CPU6502::IOStore8(u16 /* address */, u8 /* value */) {
}

void CPU6502::IOStore16(u16 /* address */, u16 /* value */) {
}

void CPU6502::Reset() {
	PC = 0xFCE2;
}
