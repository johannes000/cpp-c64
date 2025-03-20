#include "CPU6502.hpp"
#include "C64/C64.hpp"
#include <format>

CPU6502::CPU6502(C64 *c64) : Component("CPU"),
							 mCurrentAddr(0), mCurrentAddrUnmod(0),
							 mIsBranch(false),
							 mC64(c64),
							 A(0), X(0), Y(0),
							 PS(0), SP(0), PC(0),
							 log(spdlog::get("CPU")) {
	assert(OpCodes.size() == 256);
	// auto f = (this->*OpCodes[3].InstFunc)();
}

CPU6502::~CPU6502() {
}

CPU6502::Instruction CPU6502::GetNextInstruction() {
	return OpCodes[mC64->load8(PC)];
}

void CPU6502::RunOneInstruction() {
	mIsBranch = false;

	mCurrentInstruction = GetNextInstruction();
	(this->*(mCurrentInstruction.InstFunc))();
	mCurrentAddr = (this->*(mCurrentInstruction.AddrFunc))();
	// LogCurrentInstruction();
	if (!mIsBranch)
		PC += mCurrentInstruction.length;
}

void CPU6502::LogCurrentInstruction() {
	log->info(GetCurrentInstructionLog());
}

std::string CPU6502::GetCurrentInstructionAddressString() const {
	std::string addrStr;
	if (mCurrentInstruction.AddrFunc == &CPU6502::Implied) {
		addrStr = "(Implied)";
	} else if (mCurrentInstruction.AddrFunc == &CPU6502::Immediate) {
		addrStr = std::format("#${:X}", mCurrentAddr);
	} else if (mCurrentInstruction.AddrFunc == &CPU6502::ZeroPage) {
		addrStr = std::format("${:X}", (u8)mCurrentAddr);
	} else if (mCurrentInstruction.AddrFunc == &CPU6502::ZeroPageX) {
		addrStr = std::format("${:X},{:X}", (u8)mCurrentAddrUnmod, X);
	} else if (mCurrentInstruction.AddrFunc == &CPU6502::ZeroPageY) {
		addrStr = std::format("${:X},{:X}", (u8)mCurrentAddrUnmod, Y);
	} else if (mCurrentInstruction.AddrFunc == &CPU6502::Absolute) {
		addrStr = std::format("${:X}", mCurrentAddr);
	} else if (mCurrentInstruction.AddrFunc == &CPU6502::AbsoluteX) {
		addrStr = std::format("${:X},{:X}", mCurrentAddrUnmod, X);
	} else if (mCurrentInstruction.AddrFunc == &CPU6502::AbsoluteY) {
		addrStr = std::format("${:X},{:X}", mCurrentAddrUnmod, Y);
	} else if (mCurrentInstruction.AddrFunc == &CPU6502::Indirect) {
		addrStr = std::format("(${:X})", mCurrentAddr);
	} else if (mCurrentInstruction.AddrFunc == &CPU6502::IndirectX) {
		addrStr = std::format("(${:X},{:X})", mCurrentAddrUnmod, X);
	} else if (mCurrentInstruction.AddrFunc == &CPU6502::IndirectY) {
		addrStr = std::format("(${:X},{:X})", mCurrentAddrUnmod, Y);
	} else if (mCurrentInstruction.AddrFunc == &CPU6502::Accumulator) {
		addrStr = std::format("{:X}", A);
	} else if (mCurrentInstruction.AddrFunc == &CPU6502::Relative) {
		addrStr = std::format("{}", (i8)mCurrentAddrUnmod);
	}
	return addrStr;
}

const std::string CPU6502::GetCurrentInstructionLog() const {

	auto addrStr = GetCurrentInstructionAddressString();
	auto registerStr = std::format("A: {:02X} | X: {:02X} | Y: {:02X} | PS: {:08b} | SP: {:02X} | PC: {:02X}",
								   A, X, Y, PS.data, SP, PC);
	// log->info("{:X} {} {}\t{}", mC64->load8(PC), mCurrentInstruction.Name, addrStr, registerStr);
	return std::format("{:X} {} {}\t{} Wert @ PC {:X}",
					   mC64->load8(PC), mCurrentInstruction.Name, addrStr, registerStr, mC64->load16(PC));
};

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
	PC = mC64->load16(0xFFFC);
	SP = 0xFD;
}

void CPU6502::UpdateCpuStateSnapshot() {
	mCpuStateSnapshot = CPUState{
		.A = A,
		.X = X,
		.Y = Y,
		.PS = PS,
		.SP = SP,
		.PC = PC};
}
