#include "CPU6502.hpp"
#include "C64\C64.hpp"

void CPU6502::ILL() {
	log->warn("Illegale Instruction");
}

void CPU6502::LDA() {
}

void CPU6502::LDX() {
	log->warn("{} Noch nicht implementiert", __FUNCTION__);
}

void CPU6502::LDY() {
	log->warn("{} Noch nicht implementiert", __FUNCTION__);
}

void CPU6502::STA() {
	log->warn("{} Noch nicht implementiert", __FUNCTION__);
}

void CPU6502::STX() {
	log->warn("{} Noch nicht implementiert", __FUNCTION__);
}

void CPU6502::STY() {
	log->warn("{} Noch nicht implementiert", __FUNCTION__);
}

void CPU6502::TAX() {
	log->warn("{} Noch nicht implementiert", __FUNCTION__);
}

void CPU6502::TAY() {
	log->warn("{} Noch nicht implementiert", __FUNCTION__);
}

void CPU6502::TXA() {
	log->warn("{} Noch nicht implementiert", __FUNCTION__);
}

void CPU6502::TYA() {
	log->warn("{} Noch nicht implementiert", __FUNCTION__);
}

void CPU6502::TSX() {
	log->warn("{} Noch nicht implementiert", __FUNCTION__);
}

void CPU6502::TXS() {
	log->warn("{} Noch nicht implementiert", __FUNCTION__);
}

void CPU6502::PHA() {
	log->warn("{} Noch nicht implementiert", __FUNCTION__);
}

void CPU6502::PHP() {
	log->warn("{} Noch nicht implementiert", __FUNCTION__);
}

void CPU6502::PLA() {
	log->warn("{} Noch nicht implementiert", __FUNCTION__);
}

void CPU6502::PLP() {
	log->warn("{} Noch nicht implementiert", __FUNCTION__);
}

void CPU6502::AND() {
	log->warn("{} Noch nicht implementiert", __FUNCTION__);
}

void CPU6502::EOR() {
	log->warn("{} Noch nicht implementiert", __FUNCTION__);
}

void CPU6502::ORA() {
	log->warn("{} Noch nicht implementiert", __FUNCTION__);
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
	log->warn("{} Noch nicht implementiert", __FUNCTION__);
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
	log->warn("{} Noch nicht implementiert", __FUNCTION__);
}

void CPU6502::CLD() {
	log->warn("{} Noch nicht implementiert", __FUNCTION__);
}

void CPU6502::CLI() {
	log->warn("{} Noch nicht implementiert", __FUNCTION__);
}

void CPU6502::CLV() {
	log->warn("{} Noch nicht implementiert", __FUNCTION__);
}

void CPU6502::SEC() {
	log->warn("{} Noch nicht implementiert", __FUNCTION__);
}

void CPU6502::SED() {
	log->warn("{} Noch nicht implementiert", __FUNCTION__);
}

void CPU6502::SEI() {
	log->warn("{} Noch nicht implementiert", __FUNCTION__);
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
	auto oldAddr = Absolute();
	auto newAddr = oldAddr + X;
	if ((newAddr & 0xFF00) != (oldAddr & 0xFF00)) {
		// TODO: add 1 to cycles if page boundary is crossed
	}
	return newAddr;
}

u16 CPU6502::AbsoluteY() {
	auto oldAddr = Absolute();
	auto newAddr = oldAddr + Y;
	if ((newAddr & 0xFF00) != (oldAddr & 0xFF00)) {
		// TODO: add 1 to cycles if page boundary is crossed
	}
	return newAddr;
}

u16 CPU6502::Indirect() {
	return mC64->load16(PC + 1);
}

u16 CPU6502::IndirectX() {
	return mC64->load16(PC + 1) + X;
}

u16 CPU6502::IndirectY() {
	return mC64->load16(PC + 1) + Y;
}

u16 CPU6502::Implied() {
	return 0;
}

u16 CPU6502::Accumulator() {
	log->warn("Accumulator Addressing. Sollte normalerweise nicht aufgerufen werden können");
	return 0;
}

u16 CPU6502::Relative() {
	mIsBranch = true;
	return PC + (i8)(mC64->load8(PC + 1));
}
