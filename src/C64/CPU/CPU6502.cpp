#include "CPU6502.hpp"
#include "C64/C64.hpp"
#include <format>

CPU6502::CPU6502(C64 *c64) : Component("CPU"),
							 mCurrentAddr(0), mCurrentAddrUnmod(0),
							 mIsJump(false),
							 mC64(c64),
							 A(0), X(0), Y(0),
							 PS(0), SP(0), PC(0) {
	assert(OpCodes.size() == 256);
	// auto f = (this->*OpCodes[3].InstFunc)();
}

CPU6502::~CPU6502() {
}

[[nodiscard]] CPU6502::Instruction CPU6502::GetNextInstruction() {
	return GetInstructionAtAddr(PC);
}

void CPU6502::RunOneInstruction() {
	mIsJump = false;
	mCurrentInstruction = GetNextInstruction();
	mCurrentAddr = (this->*(mCurrentInstruction.AddrFunc))();
	(this->*(mCurrentInstruction.InstFunc))();
	LogCurrentInstruction();
	if (!mIsJump)
		PC += mCurrentInstruction.length;
	UpdateCpuStateSnapshot();
}

void CPU6502::LogCurrentInstruction() {
	log->info(GetCurrentInstructionLog());
}

[[nodiscard]] CPU6502::Instruction CPU6502::GetInstructionAtAddr(u16 addr) const {
	return OpCodes[mC64->load8(addr)];
}

std::string CPU6502::GetCurrentInstructionAddressString() const {
	std::string addrStr;
	if (mCurrentInstruction.AddrFunc == &CPU6502::Implied) {
		addrStr = "(Impl)";
	} else if (mCurrentInstruction.AddrFunc == &CPU6502::Immediate) {
		addrStr = std::format("#${:X}", mC64->load8(mCurrentAddr));
	} else if (mCurrentInstruction.AddrFunc == &CPU6502::ZeroPage) {
		addrStr = std::format("${:X}", (u8)mCurrentAddr);
	} else if (mCurrentInstruction.AddrFunc == &CPU6502::ZeroPageX) {
		addrStr = std::format("${:X},X", (u8)mCurrentAddrUnmod);
	} else if (mCurrentInstruction.AddrFunc == &CPU6502::ZeroPageY) {
		addrStr = std::format("${:X},Y", (u8)mCurrentAddrUnmod);
	} else if (mCurrentInstruction.AddrFunc == &CPU6502::Absolute) {
		addrStr = std::format("${:X}", mCurrentAddr);
	} else if (mCurrentInstruction.AddrFunc == &CPU6502::AbsoluteX) {
		addrStr = std::format("${:X},X", mCurrentAddrUnmod);
	} else if (mCurrentInstruction.AddrFunc == &CPU6502::AbsoluteY) {
		addrStr = std::format("${:X},Y", mCurrentAddrUnmod);
	} else if (mCurrentInstruction.AddrFunc == &CPU6502::Indirect) {
		addrStr = std::format("(${:X})", mCurrentAddr);
	} else if (mCurrentInstruction.AddrFunc == &CPU6502::IndirectX) {
		addrStr = std::format("(${:X},X)", mCurrentAddrUnmod);
	} else if (mCurrentInstruction.AddrFunc == &CPU6502::IndirectY) {
		addrStr = std::format("(${:X},Y)", mCurrentAddrUnmod);
	} else if (mCurrentInstruction.AddrFunc == &CPU6502::Accumulator) {
		addrStr = std::format("A");
	} else if (mCurrentInstruction.AddrFunc == &CPU6502::Relative) {
		addrStr = std::format("R {}", (i8)mCurrentAddrUnmod);
	}
	addrStr.resize(7, ' ');
	return addrStr;
}

std::string CPU6502::GetAddressString(u16 /* addr */) const {
}

std::string CPU6502::GetImmStringAtAddress(u16 addr) const {
	std::string immStr;
	switch (GetInstruction(addr).length) {
		case 0:
			break;
		case 1:
			immStr = std::format("{:02X}", mC64->load8(addr + 1));
			break;
		case 2:
			immStr = std::format("{:02X} {:02X}", mC64->load8(addr + 1), mC64->load8(addr + 2));
			break;
		case 3:
			immStr = std::format("{:02X} {:02X} {:02X}", mC64->load8(addr + 1), mC64->load8(addr + 2), mC64->load8(addr + 3));
			break;
		default:
			break;
	}
	immStr.resize(8, ' ');
	return immStr;
}

const std::string CPU6502::GetCurrentInstructionLog() const {
	auto addrStr = GetCurrentInstructionAddressString();
	auto immStr = GetImmStringAtAddress(PC);
	auto registerStr = std::format("A: {:02X} | X: {:02X} | Y: {:02X} | PS: {:08b} | SP: {:02X}",
								   A, X, Y, PS.data, SP);
	// log->info("{:X} {} {}\t{}", mC64->load8(PC), mCurrentInstruction.Name, addrStr, registerStr);
	return std::format("{:04X} {} {} {:02X} {}\t{} ",
					   PC, immStr, mCurrentInstruction.Name, mC64->load8(PC), addrStr, registerStr);
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
	PS.data = 0;
	A = 0;
	X = 0;
	Y = 0;
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

void CPU6502::PushToStack(u8 val) {
	mC64->store8(SP--, val);
}

void CPU6502::PushAddressToStack(u16 addr) {
	PushToStack(addr & 0xFF);
	PushToStack((addr & 0xFF00) >> 8);
}

u16 CPU6502::PullAddressFromStack() {
	u8 l = PullFromStack();
	u8 h = PullFromStack();
	return h | l << 8;
}

u8 CPU6502::PullFromStack() {
	return mC64->load8(++SP);
}

u8 CPU6502::GetValAtCurrentAddr() {
	return mC64->load8(mCurrentAddr);
}

void CPU6502::StoreValAtCurrentAddr(u8 v) {
	mC64->store8(mCurrentAddr, v);
}

void CPU6502::HandleZeroFlag(u8 val) {
	PS.Z = val == 0 ? 1 : 0;
}

void CPU6502::HandleZeroFlag(u16 val) {
	PS.Z = val == 0 ? 1 : 0;
}

void CPU6502::HandleNegativeFlag(u8 val) {
	// TODO: Unittest
	PS.N = CheckBit(val, 7) ? 1 : 0;
}

void CPU6502::HandleNegativeFlag(u16 val) {
	// TODO: Unittest
	PS.N = CheckBit(val, 15) ? 1 : 0;
}

void CPU6502::HandleNZFlags(u8 val) {
	HandleNegativeFlag(val);
	HandleZeroFlag(val);
}

void CPU6502::HandleNZFlags(u16 val) {
	HandleNegativeFlag(val);
	HandleZeroFlag(val);
}

void CPU6502::Branch(bool f) {
	if (f) {
		PC = mCurrentAddr;
		mIsJump = true;
	}
}
