#include "CPU6502.hpp"
#include <format>

CPU6502::CPU6502(Bus *bus) : Component("CPU"),
							 mCurrentAddr(0), mCurrentAddrUnmod(0),
							 mCurrentInstructionIsJump(false),
							 mBus(bus),
							 A(0), X(0), Y(0),
							 PS(0), SP(0), PC(0) {
	assert(OpCodes.size() == 256);
	// auto f = (this->*OpCodes[3].InstFunc)();
}

CPU6502::~CPU6502() {
}

[[nodiscard]] auto CPU6502::GetNextInstruction() -> CPU6502::Instruction {
	return GetInstructionAtAddr(PC);
}

auto CPU6502::RunOneInstruction() -> void {
	mCurrentInstructionIsJump = false;
	mCurrentInstruction = GetNextInstruction();
	mCurrentAddr = (this->*(mCurrentInstruction.AddrFunc))();
	(this->*(mCurrentInstruction.InstFunc))();
	if (log->should_log(spdlog::level::info))
		LogCurrentInstruction();
	if (!mCurrentInstructionIsJump)
		PC += mCurrentInstruction.length;
	UpdateCpuStateSnapshot();
}

auto CPU6502::LogCurrentInstruction() -> void {
	log->info(GetCurrentInstructionLog());
}

[[nodiscard]] auto CPU6502::GetInstructionAtAddr(u16 addr) const -> CPU6502::Instruction {
	return OpCodes[mBus->load8(addr)];
}

auto CPU6502::GetCurrentInstructionAddressString() const -> std::string {
	std::string addrStr;
	if (mCurrentInstruction.AddrFunc == &CPU6502::Implied) {
		addrStr = "(Impl)";
	} else if (mCurrentInstruction.AddrFunc == &CPU6502::Immediate) {
		addrStr = std::format("#${:X}", mBus->load8(mCurrentAddr));
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
		addrStr = std::format("(${:X})", mCurrentAddrUnmod);
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

auto CPU6502::GetAddressString(u16 /* addr */) const -> std::string {
	return "";
}

auto CPU6502::GetImmStringAtAddress(u16 addr) const -> std::string {
	std::string immStr;
	// length enthält das Opcode-Byte, angezeigt werden nur die Operanden
	switch (GetInstructionAtAddr(addr).length) {
		case 2:
			immStr = std::format("{:02X}", mBus->load8(addr + 1));
			break;
		case 3:
			immStr = std::format("{:02X} {:02X}", mBus->load8(addr + 1), mBus->load8(addr + 2));
			break;
		default:
			break;
	}
	immStr.resize(8, ' ');
	return immStr;
}

auto CPU6502::GetCurrentInstructionLog() const -> const std::string {
	auto addrStr = GetCurrentInstructionAddressString();
	auto immStr = GetImmStringAtAddress(PC);
	auto registerStr = std::format("A: {:02X} | X: {:02X} | Y: {:02X} | PS: {:08b} | SP: {:02X}",
								   A, X, Y, PS.data, SP);
	// log->info("{:X} {} {}\t{}", mBus->load8(PC), mCurrentInstruction.Name, addrStr, registerStr);
	return std::format("{:04X} {} {} {:02X} {}\t{} ",
					   PC, immStr, mCurrentInstruction.Name, mBus->load8(PC), addrStr, registerStr);
};

auto CPU6502::IOLoad8(u16 /* address */) const -> u8 {
	return 0;
}

auto CPU6502::IOLoad16(u16 /* address */) const -> u16 {
	return 0;
}

auto CPU6502::IOStore8(u16 /* address */, u8 /* value */) -> void {
}

auto CPU6502::IOStore16(u16 /* address */, u16 /* value */) -> void {
}

auto CPU6502::Reset() -> void {
	PC = Load16(0xFFFC);
	SP = 0xFD;
	PS.data = 0x24; // I gesetzt, Bit 5 immer 1
	A = 0;
	X = 0;
	Y = 0;
}

auto CPU6502::UpdateCpuStateSnapshot() -> void {
	mCpuStateSnapshot = CPUState{
		.A = A,
		.X = X,
		.Y = Y,
		.PS = PS,
		.SP = SP,
		.PC = PC};
}

auto CPU6502::PushToStack(u8 val) -> void {
	// Der Stack liegt in Page 1 ($0100-$01FF)
	mBus->store8(0x0100 | SP--, val);
}

auto CPU6502::PushAddressToStack(u16 addr) -> void {
	// High-Byte zuerst, damit es an der höheren Adresse liegt
	PushToStack((addr & 0xFF00) >> 8);
	PushToStack(addr & 0xFF);
}

auto CPU6502::PullAddressFromStack() -> u16 {
	u8 l = PullFromStack();
	u8 h = PullFromStack();
	return l | h << 8;
}

auto CPU6502::PullFromStack() -> u8 {
	return mBus->load8(0x0100 | ++SP);
}

auto CPU6502::GetValAtCurrentAddr() -> u8 {
	return mBus->load8(mCurrentAddr);
}

auto CPU6502::StoreValAtCurrentAddr(u8 v) -> void {
	mBus->store8(mCurrentAddr, v);
}

auto CPU6502::Load16(u16 addr) const -> u16 {
	return mBus->load8(addr) | mBus->load8(addr + 1) << 8;
}

auto CPU6502::Load16ZeroPage(u8 addr) const -> u16 {
	// Zeiger in der Zero Page laufen bei $FF auf $00 über
	return mBus->load8(addr) | mBus->load8((u8)(addr + 1)) << 8;
}

auto CPU6502::HandleZeroFlag(u8 val) -> void {
	PS.Z = val == 0 ? 1 : 0;
}

auto CPU6502::HandleZeroFlag(u16 val) -> void {
	PS.Z = val == 0 ? 1 : 0;
}

auto CPU6502::HandleNegativeFlag(u8 val) -> void {
	// TODO: Unittest
	PS.N = CheckBit(val, 7) ? 1 : 0;
}

auto CPU6502::HandleNegativeFlag(u16 val) -> void {
	// TODO: Unittest
	PS.N = CheckBit(val, 15) ? 1 : 0;
}

auto CPU6502::HandleNZFlags(u8 val) -> void {
	HandleNegativeFlag(val);
	HandleZeroFlag(val);
}

auto CPU6502::HandleNZFlags(u16 val) -> void {
	HandleNegativeFlag(val);
	HandleZeroFlag(val);
}

auto CPU6502::Branch(bool f) -> void {
	if (f) {
		PC = mCurrentAddr;
		mCurrentInstructionIsJump = true;
	}
}
