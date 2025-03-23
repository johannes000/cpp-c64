#include "CPU6502.hpp"
#include "C64\C64.hpp"

void CPU6502::ILL() {
	log->warn("Illegale Instruction");
}

void CPU6502::LDA() {
	// 	Load Accumulator with Memory
	// M -> A
	A = mC64->load8(mCurrentAddr);
	HandleNegativeFlag(A);
	HandleZeroFlag(A);
}

void CPU6502::LDX() {
	// 	Load Index X with Memory
	// M -> X
	X = mC64->load8(mCurrentAddr);
	HandleNegativeFlag(X);
	HandleZeroFlag(X);
}

void CPU6502::LDY() {
	// Load Index Y with Memory
	// M -> Y
	Y = mC64->load8(mCurrentAddr);
	HandleNegativeFlag(Y);
	HandleZeroFlag(Y);
}

void CPU6502::STA() {
	// Store Accumulator in Memory
	// A -> M
	A = mC64->load8(mCurrentAddr);
}

void CPU6502::STX() {
	// Store Index X in Memory
	// X->M
	X = mC64->load8(mCurrentAddr);
}

void CPU6502::STY() {
	// Sore Index Y in Memory
	// Y->M
	Y = mC64->load8(mCurrentAddr);
}

void CPU6502::TAX() {
	// Transfer Accumulator to Index X
	// A->X
	X = A;
	HandleNegativeFlag(A);
	HandleZeroFlag(A);
}

void CPU6502::TAY() {
	// Transfer Accumulator to Index Y
	// A -> Y
	Y = A;
	HandleNegativeFlag(A);
	HandleZeroFlag(A);
}

void CPU6502::TXA() {
	// Transfer Index X to Accumulator
	// X -> A
	A = X;
	HandleNegativeFlag(A);
	HandleZeroFlag(A);
}

void CPU6502::TYA() {
	// Transfer Index Y to Accumulator
	// Y -> A
	A = Y;
	HandleNegativeFlag(A);
	HandleZeroFlag(A);
}

void CPU6502::TSX() {
	// Transfer Index X to Stack Register
	// X -> SP
	X = SP;
	HandleNegativeFlag(X);
	HandleZeroFlag(X);
}

void CPU6502::TXS() {
	// Transfer Index X to Stack Register
	// X -> SP
	SP = X;
}

void CPU6502::PHA() {
	// Push Accumulator on Stack
	// push A
	PushToStack(A);
}

void CPU6502::PHP() {
	// Push Processor Status on Stack
	// The status register will be pushed with the break
	// flag and bit 5 set to 1.
	// push SR
	PS.B = 1;
	PushToStack(PS.data);
}

void CPU6502::PLA() {
	// Pull Accumulator from Stack
	// pull A
	A = PullFromStack();
	HandleNegativeFlag(A);
	HandleZeroFlag(A);
}

void CPU6502::PLP() {

	// Pull Processor Status from Stack
	// The status register will be pulled with the break
	// flag and bit 5 ignored.
	// pull SR
	auto B = PS.B;
	PS.data = PullFromStack();
	PS.B = B;
}

void CPU6502::AND() {
	// AND Memory with Accumulator
	// A AND M -> A
	A &= mC64->load8(mCurrentAddr);
	HandleNegativeFlag(A);
	HandleZeroFlag(A);
}

void CPU6502::EOR() {
	// Exclusive-OR Memory with Accumulator
	// A EOR M -> A
	A ^= mC64->load8(mCurrentAddr);
	HandleNegativeFlag(A);
	HandleZeroFlag(A);
}

void CPU6502::ORA() {
	// OR Memory with Accumulator
	// A OR M -> A
	A |= mC64->load8(mCurrentAddr);
	HandleNegativeFlag(A);
	HandleZeroFlag(A);
}

void CPU6502::BIT() {
	log->warn("{} Noch nicht implementiert", __FUNCTION__);
}

void CPU6502::ADC() {
	log->warn("{} Noch nicht implementiert", __FUNCTION__);
}

void CPU6502::SBC() {
	log->warn("{} Noch nicht implementiert", __FUNCTION__);
}

void CPU6502::CMP() {
	// Compare sets flags as if a subtraction had been carried out.
	// If the value in the accumulator is equal or greater than the compared value, the Carry will be set.
	// The equal (Z) and negative (N) flags will be set based on equality or lack thereof and the sign (i.e. A>=$80) of the accumulator.
	// TODO: + add 1 cycle if page boundary crossed
	auto val = mC64->load8(mCurrentAddr);
	log->warn("{} Noch nicht implementiert", __FUNCTION__);
}

void CPU6502::CPX() {
	log->warn("{} Noch nicht implementiert", __FUNCTION__);
}

void CPU6502::CPY() {
	log->warn("{} Noch nicht implementiert", __FUNCTION__);
}

void CPU6502::INC() {
	log->warn("{} Noch nicht implementiert", __FUNCTION__);
}

void CPU6502::INX() {
	log->warn("{} Noch nicht implementiert", __FUNCTION__);
}

void CPU6502::INY() {
	log->warn("{} Noch nicht implementiert", __FUNCTION__);
}

void CPU6502::DEC() {
	log->warn("{} Noch nicht implementiert", __FUNCTION__);
}

void CPU6502::DEX() {
	log->warn("{} Noch nicht implementiert", __FUNCTION__);
}

void CPU6502::DEY() {
	log->warn("{} Noch nicht implementiert", __FUNCTION__);
}

void CPU6502::ASL() {
	log->warn("{} Noch nicht implementiert", __FUNCTION__);
}

void CPU6502::LSR() {
	log->warn("{} Noch nicht implementiert", __FUNCTION__);
}

void CPU6502::ROL() {
	log->warn("{} Noch nicht implementiert", __FUNCTION__);
}

void CPU6502::ROR() {
	log->warn("{} Noch nicht implementiert", __FUNCTION__);
}

void CPU6502::JMP() {
	log->warn("{} Noch nicht implementiert", __FUNCTION__);
}

void CPU6502::JSR() {
	// Jump to New Location Saving Return Address
	// Pushed PC - 1 relativ zur nächsten Inst
	PushAddressToStack(PC + mCurrentInstruction.length - 1);
	PC = mCurrentAddr;
	mIsJump = true;
}

void CPU6502::RTS() {
	log->warn("{} Noch nicht implementiert", __FUNCTION__);
}

void CPU6502::BCC() {
	log->warn("{} Noch nicht implementiert", __FUNCTION__);
}

void CPU6502::BCS() {
	log->warn("{} Noch nicht implementiert", __FUNCTION__);
}

void CPU6502::BEQ() {
	log->warn("{} Noch nicht implementiert", __FUNCTION__);
}

void CPU6502::BMI() {
	log->warn("{} Noch nicht implementiert", __FUNCTION__);
}

void CPU6502::BNE() {
	log->warn("{} Noch nicht implementiert", __FUNCTION__);
}

void CPU6502::BPL() {
	log->warn("{} Noch nicht implementiert", __FUNCTION__);
}

void CPU6502::BVC() {
	log->warn("{} Noch nicht implementiert", __FUNCTION__);
}

void CPU6502::BVS() {
	log->warn("{} Noch nicht implementiert", __FUNCTION__);
}

void CPU6502::CLC() {
	// Clear Carry Flag
	// 0 -> C
	PS.C = 0;
}

void CPU6502::CLD() {
	// Clear Decimal Mode
	// 0 -> D
	PS.D = 0;
}

void CPU6502::CLI() {
	// Clear Interrupt Disable Bit
	// 0 -> I
	PS.I = 0;
}

void CPU6502::CLV() {
	// Clear Overflow Flag
	// 0 -> V
	PS.V = 0;
}

void CPU6502::SEC() {
	log->warn("{} Noch nicht implementiert", __FUNCTION__);
}

void CPU6502::SED() {
	log->warn("{} Noch nicht implementiert", __FUNCTION__);
}

void CPU6502::SEI() {
	PS.I = 1;
}

void CPU6502::BRK() {
	log->warn("{} Noch nicht implementiert", __FUNCTION__);
}

void CPU6502::NOP() {
	log->warn("{} Noch nicht implementiert", __FUNCTION__);
}

void CPU6502::RTI() {
	log->warn("{} Noch nicht implementiert", __FUNCTION__);
}

u16 CPU6502::Immediate() {
	return (PC + 1);
}

u16 CPU6502::ZeroPage() {
	return Immediate() & 0x00FF;
}

u16 CPU6502::ZeroPageX() {
	return ((u8)(Immediate() + X));
}

u16 CPU6502::ZeroPageY() {
	return ((u8)(Immediate() + Y));
}

u16 CPU6502::Absolute() {
	return mC64->load16(PC + 1);
}

u16 CPU6502::AbsoluteX() {
	mCurrentAddrUnmod = Absolute();
	auto newAddr = mCurrentAddrUnmod + X;
	if ((newAddr & 0xFF00) != (mCurrentAddrUnmod & 0xFF00)) {
		// TODO: add 1 to cycles if page boundary is crossed
	}
	return newAddr;
}

u16 CPU6502::AbsoluteY() {
	mCurrentAddrUnmod = Absolute();
	auto newAddr = mCurrentAddrUnmod + Y;
	if ((newAddr & 0xFF00) != (mCurrentAddrUnmod & 0xFF00)) {
		// TODO: add 1 to cycles if page boundary is crossed
	}
	return newAddr;
}

u16 CPU6502::Indirect() {
	return mC64->load16(PC + 1);
}

u16 CPU6502::IndirectX() {
	mCurrentAddrUnmod = mC64->load16(PC + 1);
	return mCurrentAddrUnmod + X;
}

u16 CPU6502::IndirectY() {
	mCurrentAddrUnmod = mC64->load16(PC + 1);
	return mCurrentAddrUnmod + Y;
}

u16 CPU6502::Implied() {
	return 0;
}

u16 CPU6502::Accumulator() {
	log->warn("Accumulator Addressing. Sollte normalerweise nicht aufgerufen werden können");
	return 0;
}

u16 CPU6502::Relative() {
	mIsJump = true;
	mCurrentAddrUnmod = mC64->load8(PC + 1);
	return PC + (i8)mCurrentAddrUnmod;
}
