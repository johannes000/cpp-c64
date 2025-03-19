#pragma once
#include "Utility\Includes.hpp"
#include "C64\Component.hpp"
#include "C64\Memory\Memory.hpp"

class C64;

class CPU6502 : public Component {
public:
	struct Instruction {
		std::string Name;
		u8 length{0};
		u8 cycles{0};
		void (CPU6502::*InstFunc)() = nullptr;
		u16 (CPU6502::*AddrFunc)() = nullptr;
	};

public:
	CPU6502(C64 *c64);
	~CPU6502();

	Instruction GetNextInstruction();
	u16 mCurrentAddr;
	u16 mCurrentAddrUnmod; // Für Debugger

	bool mIsBranch;

	void RunOneInstruction();
	void LogCurrentInstruction();
	const std::string GetCurrentInstructionLog() const;

	u8 IOLoad8(u16 address) const override;
	u16 IOLoad16(u16 address) const override;

	void IOStore8(u16 address, u8 value) override;
	void IOStore16(u16 address, u16 value) override;

	void Reset();

private:
	C64 *mC64;

	Instruction mCurrentInstruction;

	// Accumulator
	u8 A;
	// Index Register X
	u8 X;
	// Index Register Y
	u8 Y;
	// Processor Status
	union Processor_Status {
		u8 data;
		struct {
			u8 C : 1;	   // Carry Flag: 1 on unsigned overflow
			u8 Z : 1;	   // Zero Flag: 1 when all bits of a result are 0
			u8 I : 1;	   // Interrupt Disable Flag: when 1, no interrupt will occur (except BRK and NMI)
			u8 D : 1;	   // Decimal Mode Flag: 1 when CPU is in Decimal Mode
			u8 B : 1;	   // Break Flag: 1 when pushed by instructions (BRK / PHP) and 0 when pushed by interrupts (NMI / IRQ)
			u8 unused : 1; // Unused: always set to 1
			u8 V : 1;	   // Overflow Flag: 1 on signed overflow
			u8 N : 1;	   // Negative Flag: 1 when result is negative
		};
	} PS;
	// Stack Pointer
	u8 SP;
	// Program Counter
	u16 PC;

	std::shared_ptr<spdlog::logger> log;

private:
	const std::vector<Instruction> OpCodes{
		{"BRK", 1, 7, &CPU6502::BRK, &CPU6502::Implied},	 // 0x00
		{"ORA", 2, 6, &CPU6502::ORA, &CPU6502::IndirectX},	 // 0x01
		{"ILL", 1, 0, &CPU6502::ILL, &CPU6502::Implied},	 // 0x02 (Illegal)
		{"ILL", 1, 0, &CPU6502::ILL, &CPU6502::Implied},	 // 0x03 (Illegal)
		{"NOP", 2, 3, &CPU6502::NOP, &CPU6502::ZeroPage},	 // 0x04 (Illegal, oft als NOP behandelt)
		{"ORA", 2, 3, &CPU6502::ORA, &CPU6502::ZeroPage},	 // 0x05
		{"ASL", 2, 5, &CPU6502::ASL, &CPU6502::ZeroPage},	 // 0x06
		{"ILL", 1, 0, &CPU6502::ILL, &CPU6502::Implied},	 // 0x07 (Illegal)
		{"PHP", 1, 3, &CPU6502::PHP, &CPU6502::Implied},	 // 0x08
		{"ORA", 2, 2, &CPU6502::ORA, &CPU6502::Immediate},	 // 0x09
		{"ASL", 1, 2, &CPU6502::ASL, &CPU6502::Accumulator}, // 0x0A
		{"ILL", 1, 0, &CPU6502::ILL, &CPU6502::Implied},	 // 0x0B (Illegal)
		{"NOP", 3, 4, &CPU6502::NOP, &CPU6502::Absolute},	 // 0x0C (Illegal, oft als NOP behandelt)
		{"ORA", 3, 4, &CPU6502::ORA, &CPU6502::Absolute},	 // 0x0D
		{"ASL", 3, 6, &CPU6502::ASL, &CPU6502::Absolute},	 // 0x0E
		{"ILL", 1, 0, &CPU6502::ILL, &CPU6502::Implied},	 // 0x0F (Illegal)

		{"BPL", 2, 2, &CPU6502::BPL, &CPU6502::Relative},  // 0x10
		{"ORA", 2, 5, &CPU6502::ORA, &CPU6502::IndirectY}, // 0x11
		{"ILL", 1, 0, &CPU6502::ILL, &CPU6502::Implied},   // 0x12 (Illegal)
		{"ILL", 1, 0, &CPU6502::ILL, &CPU6502::Implied},   // 0x13 (Illegal)
		{"NOP", 2, 4, &CPU6502::NOP, &CPU6502::ZeroPageX}, // 0x14 (Illegal, oft als NOP behandelt)
		{"ORA", 2, 4, &CPU6502::ORA, &CPU6502::ZeroPageX}, // 0x15
		{"ASL", 2, 6, &CPU6502::ASL, &CPU6502::ZeroPageX}, // 0x16
		{"ILL", 1, 0, &CPU6502::ILL, &CPU6502::Implied},   // 0x17 (Illegal)
		{"CLC", 1, 2, &CPU6502::CLC, &CPU6502::Implied},   // 0x18
		{"ORA", 3, 4, &CPU6502::ORA, &CPU6502::AbsoluteY}, // 0x19
		{"NOP", 1, 2, &CPU6502::NOP, &CPU6502::Implied},   // 0x1A (Illegal, oft als NOP behandelt)
		{"ILL", 1, 0, &CPU6502::ILL, &CPU6502::Implied},   // 0x1B (Illegal)
		{"NOP", 3, 4, &CPU6502::NOP, &CPU6502::AbsoluteX}, // 0x1C (Illegal, oft als NOP behandelt)
		{"ORA", 3, 4, &CPU6502::ORA, &CPU6502::AbsoluteX}, // 0x1D
		{"ASL", 3, 7, &CPU6502::ASL, &CPU6502::AbsoluteX}, // 0x1E
		{"ILL", 1, 0, &CPU6502::ILL, &CPU6502::Implied},   // 0x1F (Illegal)

		{"JSR", 3, 6, &CPU6502::JSR, &CPU6502::Absolute},	 // 0x20
		{"AND", 2, 6, &CPU6502::AND, &CPU6502::IndirectX},	 // 0x21
		{"ILL", 1, 0, &CPU6502::ILL, &CPU6502::Implied},	 // 0x22 (Illegal)
		{"ILL", 1, 0, &CPU6502::ILL, &CPU6502::Implied},	 // 0x23 (Illegal)
		{"BIT", 2, 3, &CPU6502::BIT, &CPU6502::ZeroPage},	 // 0x24
		{"AND", 2, 3, &CPU6502::AND, &CPU6502::ZeroPage},	 // 0x25
		{"ROL", 2, 5, &CPU6502::ROL, &CPU6502::ZeroPage},	 // 0x26
		{"ILL", 1, 0, &CPU6502::ILL, &CPU6502::Implied},	 // 0x27 (Illegal)
		{"PLP", 1, 4, &CPU6502::PLP, &CPU6502::Implied},	 // 0x28
		{"AND", 2, 2, &CPU6502::AND, &CPU6502::Immediate},	 // 0x29
		{"ROL", 1, 2, &CPU6502::ROL, &CPU6502::Accumulator}, // 0x2A
		{"ILL", 1, 0, &CPU6502::ILL, &CPU6502::Implied},	 // 0x2B (Illegal)
		{"BIT", 3, 4, &CPU6502::BIT, &CPU6502::Absolute},	 // 0x2C
		{"AND", 3, 4, &CPU6502::AND, &CPU6502::Absolute},	 // 0x2D
		{"ROL", 3, 6, &CPU6502::ROL, &CPU6502::Absolute},	 // 0x2E
		{"ILL", 1, 0, &CPU6502::ILL, &CPU6502::Implied},	 // 0x2F (Illegal)

		{"BMI", 2, 2, &CPU6502::BMI, &CPU6502::Relative},  // 0x30
		{"AND", 2, 5, &CPU6502::AND, &CPU6502::IndirectY}, // 0x31
		{"ILL", 1, 0, &CPU6502::ILL, &CPU6502::Implied},   // 0x32 (Illegal)
		{"ILL", 1, 0, &CPU6502::ILL, &CPU6502::Implied},   // 0x33 (Illegal)
		{"NOP", 2, 4, &CPU6502::NOP, &CPU6502::ZeroPageX}, // 0x34 (Illegal, oft als NOP behandelt)
		{"AND", 2, 4, &CPU6502::AND, &CPU6502::ZeroPageX}, // 0x35
		{"ROL", 2, 6, &CPU6502::ROL, &CPU6502::ZeroPageX}, // 0x36
		{"ILL", 1, 0, &CPU6502::ILL, &CPU6502::Implied},   // 0x37 (Illegal)
		{"SEC", 1, 2, &CPU6502::SEC, &CPU6502::Implied},   // 0x38
		{"AND", 3, 4, &CPU6502::AND, &CPU6502::AbsoluteY}, // 0x39
		{"NOP", 1, 2, &CPU6502::NOP, &CPU6502::Implied},   // 0x3A (Illegal, oft als NOP behandelt)
		{"ILL", 1, 0, &CPU6502::ILL, &CPU6502::Implied},   // 0x3B (Illegal)
		{"NOP", 3, 4, &CPU6502::NOP, &CPU6502::AbsoluteX}, // 0x3C (Illegal, oft als NOP behandelt)
		{"AND", 3, 4, &CPU6502::AND, &CPU6502::AbsoluteX}, // 0x3D
		{"ROL", 3, 7, &CPU6502::ROL, &CPU6502::AbsoluteX}, // 0x3E
		{"ILL", 1, 0, &CPU6502::ILL, &CPU6502::Implied},   // 0x3F (Illegal)

		{"RTI", 1, 6, &CPU6502::RTI, &CPU6502::Implied},	 // 0x40
		{"EOR", 2, 6, &CPU6502::EOR, &CPU6502::IndirectX},	 // 0x41
		{"ILL", 1, 0, &CPU6502::ILL, &CPU6502::Implied},	 // 0x42 (Illegal)
		{"ILL", 1, 0, &CPU6502::ILL, &CPU6502::Implied},	 // 0x43 (Illegal)
		{"NOP", 2, 3, &CPU6502::NOP, &CPU6502::ZeroPage},	 // 0x44 (Illegal, oft als NOP behandelt)
		{"EOR", 2, 3, &CPU6502::EOR, &CPU6502::ZeroPage},	 // 0x45
		{"LSR", 2, 5, &CPU6502::LSR, &CPU6502::ZeroPage},	 // 0x46
		{"ILL", 1, 0, &CPU6502::ILL, &CPU6502::Implied},	 // 0x47 (Illegal)
		{"PHA", 1, 3, &CPU6502::PHA, &CPU6502::Implied},	 // 0x48
		{"EOR", 2, 2, &CPU6502::EOR, &CPU6502::Immediate},	 // 0x49
		{"LSR", 1, 2, &CPU6502::LSR, &CPU6502::Accumulator}, // 0x4A
		{"ILL", 1, 0, &CPU6502::ILL, &CPU6502::Implied},	 // 0x4B (Illegal)
		{"JMP", 3, 3, &CPU6502::JMP, &CPU6502::Absolute},	 // 0x4C
		{"EOR", 3, 4, &CPU6502::EOR, &CPU6502::Absolute},	 // 0x4D
		{"LSR", 3, 6, &CPU6502::LSR, &CPU6502::Absolute},	 // 0x4E
		{"ILL", 1, 0, &CPU6502::ILL, &CPU6502::Implied},	 // 0x4F (Illegal)

		{"BVC", 2, 2, &CPU6502::BVC, &CPU6502::Relative},  // 0x50
		{"EOR", 2, 5, &CPU6502::EOR, &CPU6502::IndirectY}, // 0x51
		{"ILL", 1, 0, &CPU6502::ILL, &CPU6502::Implied},   // 0x52 (Illegal)
		{"ILL", 1, 0, &CPU6502::ILL, &CPU6502::Implied},   // 0x53 (Illegal)
		{"NOP", 2, 4, &CPU6502::NOP, &CPU6502::ZeroPageX}, // 0x54 (Illegal, oft als NOP behandelt)
		{"EOR", 2, 4, &CPU6502::EOR, &CPU6502::ZeroPageX}, // 0x55
		{"LSR", 2, 6, &CPU6502::LSR, &CPU6502::ZeroPageX}, // 0x56
		{"ILL", 1, 0, &CPU6502::ILL, &CPU6502::Implied},   // 0x57 (Illegal)
		{"CLI", 1, 2, &CPU6502::CLI, &CPU6502::Implied},   // 0x58
		{"EOR", 3, 4, &CPU6502::EOR, &CPU6502::AbsoluteY}, // 0x59
		{"NOP", 1, 2, &CPU6502::NOP, &CPU6502::Implied},   // 0x5A (Illegal, oft als NOP behandelt)
		{"ILL", 1, 0, &CPU6502::ILL, &CPU6502::Implied},   // 0x5B (Illegal)
		{"NOP", 3, 4, &CPU6502::NOP, &CPU6502::AbsoluteX}, // 0x5C (Illegal, oft als NOP behandelt)
		{"EOR", 3, 4, &CPU6502::EOR, &CPU6502::AbsoluteX}, // 0x5D
		{"LSR", 3, 7, &CPU6502::LSR, &CPU6502::AbsoluteX}, // 0x5E
		{"ILL", 1, 0, &CPU6502::ILL, &CPU6502::Implied},   // 0x5F (Illegal)

		{"RTS", 1, 6, &CPU6502::RTS, &CPU6502::Implied},	 // 0x60
		{"ADC", 2, 6, &CPU6502::ADC, &CPU6502::IndirectX},	 // 0x61
		{"ILL", 1, 0, &CPU6502::ILL, &CPU6502::Implied},	 // 0x62 (Illegal)
		{"ILL", 1, 0, &CPU6502::ILL, &CPU6502::Implied},	 // 0x63 (Illegal)
		{"NOP", 2, 3, &CPU6502::NOP, &CPU6502::ZeroPage},	 // 0x64 (Illegal, oft als NOP behandelt)
		{"ADC", 2, 3, &CPU6502::ADC, &CPU6502::ZeroPage},	 // 0x65
		{"ROR", 2, 5, &CPU6502::ROR, &CPU6502::ZeroPage},	 // 0x66
		{"ILL", 1, 0, &CPU6502::ILL, &CPU6502::Implied},	 // 0x67 (Illegal)
		{"PLA", 1, 4, &CPU6502::PLA, &CPU6502::Implied},	 // 0x68
		{"ADC", 2, 2, &CPU6502::ADC, &CPU6502::Immediate},	 // 0x69
		{"ROR", 1, 2, &CPU6502::ROR, &CPU6502::Accumulator}, // 0x6A
		{"ILL", 1, 0, &CPU6502::ILL, &CPU6502::Implied},	 // 0x6B (Illegal)
		{"JMP", 3, 5, &CPU6502::JMP, &CPU6502::Indirect},	 // 0x6C
		{"ADC", 3, 4, &CPU6502::ADC, &CPU6502::Absolute},	 // 0x6D
		{"ROR", 3, 6, &CPU6502::ROR, &CPU6502::Absolute},	 // 0x6E
		{"ILL", 1, 0, &CPU6502::ILL, &CPU6502::Implied},	 // 0x6F (Illegal)

		{"BVS", 2, 2, &CPU6502::BVS, &CPU6502::Relative},  // 0x70
		{"ADC", 2, 5, &CPU6502::ADC, &CPU6502::IndirectY}, // 0x71
		{"ILL", 1, 0, &CPU6502::ILL, &CPU6502::Implied},   // 0x72 (Illegal)
		{"ILL", 1, 0, &CPU6502::ILL, &CPU6502::Implied},   // 0x73 (Illegal)
		{"NOP", 2, 4, &CPU6502::NOP, &CPU6502::ZeroPageX}, // 0x74 (Illegal, oft als NOP behandelt)
		{"ADC", 2, 4, &CPU6502::ADC, &CPU6502::ZeroPageX}, // 0x75
		{"ROR", 2, 6, &CPU6502::ROR, &CPU6502::ZeroPageX}, // 0x76
		{"ILL", 1, 0, &CPU6502::ILL, &CPU6502::Implied},   // 0x77 (Illegal)
		{"SEI", 1, 2, &CPU6502::SEI, &CPU6502::Implied},   // 0x78
		{"ADC", 3, 4, &CPU6502::ADC, &CPU6502::AbsoluteY}, // 0x79
		{"NOP", 1, 2, &CPU6502::NOP, &CPU6502::Implied},   // 0x7A (Illegal, oft als NOP behandelt)
		{"ILL", 1, 0, &CPU6502::ILL, &CPU6502::Implied},   // 0x7B (Illegal)
		{"NOP", 3, 4, &CPU6502::NOP, &CPU6502::AbsoluteX}, // 0x7C (Illegal, oft als NOP behandelt)
		{"ADC", 3, 4, &CPU6502::ADC, &CPU6502::AbsoluteX}, // 0x7D
		{"ROR", 3, 7, &CPU6502::ROR, &CPU6502::AbsoluteX}, // 0x7E
		{"ILL", 1, 0, &CPU6502::ILL, &CPU6502::Implied},   // 0x7F (Illegal)

		{"NOP", 2, 2, &CPU6502::NOP, &CPU6502::Immediate}, // 0x80 (Illegal, oft als NOP behandelt)
		{"STA", 2, 6, &CPU6502::STA, &CPU6502::IndirectX}, // 0x81
		{"NOP", 2, 2, &CPU6502::NOP, &CPU6502::Immediate}, // 0x82 (Illegal, oft als NOP behandelt)
		{"ILL", 1, 0, &CPU6502::ILL, &CPU6502::Implied},   // 0x83 (Illegal)
		{"STY", 2, 3, &CPU6502::STY, &CPU6502::ZeroPage},  // 0x84
		{"STA", 2, 3, &CPU6502::STA, &CPU6502::ZeroPage},  // 0x85
		{"STX", 2, 3, &CPU6502::STX, &CPU6502::ZeroPage},  // 0x86
		{"ILL", 1, 0, &CPU6502::ILL, &CPU6502::Implied},   // 0x87 (Illegal)
		{"DEY", 1, 2, &CPU6502::DEY, &CPU6502::Implied},   // 0x88
		{"NOP", 2, 2, &CPU6502::NOP, &CPU6502::Immediate}, // 0x89 (Illegal, oft als NOP behandelt)
		{"TXA", 1, 2, &CPU6502::TXA, &CPU6502::Implied},   // 0x8A
		{"ILL", 1, 0, &CPU6502::ILL, &CPU6502::Implied},   // 0x8B (Illegal)
		{"STY", 3, 4, &CPU6502::STY, &CPU6502::Absolute},  // 0x8C
		{"STA", 3, 4, &CPU6502::STA, &CPU6502::Absolute},  // 0x8D
		{"STX", 3, 4, &CPU6502::STX, &CPU6502::Absolute},  // 0x8E
		{"ILL", 1, 0, &CPU6502::ILL, &CPU6502::Implied},   // 0x8F (Illegal)

		{"BCC", 2, 2, &CPU6502::BCC, &CPU6502::Relative},  // 0x90
		{"STA", 2, 6, &CPU6502::STA, &CPU6502::IndirectY}, // 0x91
		{"ILL", 1, 0, &CPU6502::ILL, &CPU6502::Implied},   // 0x92 (Illegal)
		{"ILL", 1, 0, &CPU6502::ILL, &CPU6502::Implied},   // 0x93 (Illegal)
		{"STY", 2, 4, &CPU6502::STY, &CPU6502::ZeroPageX}, // 0x94
		{"STA", 2, 4, &CPU6502::STA, &CPU6502::ZeroPageX}, // 0x95
		{"STX", 2, 4, &CPU6502::STX, &CPU6502::ZeroPageY}, // 0x96
		{"ILL", 1, 0, &CPU6502::ILL, &CPU6502::Implied},   // 0x97 (Illegal)
		{"TYA", 1, 2, &CPU6502::TYA, &CPU6502::Implied},   // 0x98
		{"STA", 3, 5, &CPU6502::STA, &CPU6502::AbsoluteY}, // 0x99
		{"TXS", 1, 2, &CPU6502::TXS, &CPU6502::Implied},   // 0x9A
		{"ILL", 1, 0, &CPU6502::ILL, &CPU6502::Implied},   // 0x9B (Illegal)
		{"ILL", 1, 0, &CPU6502::ILL, &CPU6502::Implied},   // 0x9C (Illegal)
		{"STA", 3, 5, &CPU6502::STA, &CPU6502::AbsoluteX}, // 0x9D
		{"ILL", 1, 0, &CPU6502::ILL, &CPU6502::Implied},   // 0x9E (Illegal)
		{"ILL", 1, 0, &CPU6502::ILL, &CPU6502::Implied},   // 0x9F (Illegal)

		{"LDY", 2, 2, &CPU6502::LDY, &CPU6502::Immediate}, // 0xA0
		{"LDA", 2, 6, &CPU6502::LDA, &CPU6502::IndirectX}, // 0xA1
		{"LDX", 2, 2, &CPU6502::LDX, &CPU6502::Immediate}, // 0xA2
		{"ILL", 1, 0, &CPU6502::ILL, &CPU6502::Implied},   // 0xA3 (Illegal)
		{"LDY", 2, 3, &CPU6502::LDY, &CPU6502::ZeroPage},  // 0xA4
		{"LDA", 2, 3, &CPU6502::LDA, &CPU6502::ZeroPage},  // 0xA5
		{"LDX", 2, 3, &CPU6502::LDX, &CPU6502::ZeroPage},  // 0xA6
		{"ILL", 1, 0, &CPU6502::ILL, &CPU6502::Implied},   // 0xA7 (Illegal)
		{"TAY", 1, 2, &CPU6502::TAY, &CPU6502::Implied},   // 0xA8
		{"LDA", 2, 2, &CPU6502::LDA, &CPU6502::Immediate}, // 0xA9
		{"TAX", 1, 2, &CPU6502::TAX, &CPU6502::Implied},   // 0xAA
		{"ILL", 1, 0, &CPU6502::ILL, &CPU6502::Implied},   // 0xAB (Illegal)
		{"LDY", 3, 4, &CPU6502::LDY, &CPU6502::Absolute},  // 0xAC
		{"LDA", 3, 4, &CPU6502::LDA, &CPU6502::Absolute},  // 0xAD
		{"LDX", 3, 4, &CPU6502::LDX, &CPU6502::Absolute},  // 0xAE
		{"ILL", 1, 0, &CPU6502::ILL, &CPU6502::Implied},   // 0xAF (Illegal)

		{"BCS", 2, 2, &CPU6502::BCS, &CPU6502::Relative},  // 0xB0
		{"LDA", 2, 5, &CPU6502::LDA, &CPU6502::IndirectY}, // 0xB1
		{"ILL", 1, 0, &CPU6502::ILL, &CPU6502::Implied},   // 0xB2 (Illegal)
		{"ILL", 1, 0, &CPU6502::ILL, &CPU6502::Implied},   // 0xB3 (Illegal)
		{"LDY", 2, 4, &CPU6502::LDY, &CPU6502::ZeroPageX}, // 0xB4
		{"LDA", 2, 4, &CPU6502::LDA, &CPU6502::ZeroPageX}, // 0xB5
		{"LDX", 2, 4, &CPU6502::LDX, &CPU6502::ZeroPageY}, // 0xB6
		{"ILL", 1, 0, &CPU6502::ILL, &CPU6502::Implied},   // 0xB7 (Illegal)
		{"CLV", 1, 2, &CPU6502::CLV, &CPU6502::Implied},   // 0xB8
		{"LDA", 3, 4, &CPU6502::LDA, &CPU6502::AbsoluteY}, // 0xB9
		{"TSX", 1, 2, &CPU6502::TSX, &CPU6502::Implied},   // 0xBA
		{"ILL", 1, 0, &CPU6502::ILL, &CPU6502::Implied},   // 0xBB (Illegal)
		{"LDY", 3, 4, &CPU6502::LDY, &CPU6502::AbsoluteX}, // 0xBC
		{"LDA", 3, 4, &CPU6502::LDA, &CPU6502::AbsoluteX}, // 0xBD
		{"LDX", 3, 4, &CPU6502::LDX, &CPU6502::AbsoluteY}, // 0xBE
		{"ILL", 1, 0, &CPU6502::ILL, &CPU6502::Implied},   // 0xBF (Illegal)

		{"CPY", 2, 2, &CPU6502::CPY, &CPU6502::Immediate}, // 0xC0
		{"CMP", 2, 6, &CPU6502::CMP, &CPU6502::IndirectX}, // 0xC1
		{"NOP", 2, 2, &CPU6502::NOP, &CPU6502::Immediate}, // 0xC2 (Illegal, oft als NOP behandelt)
		{"ILL", 1, 0, &CPU6502::ILL, &CPU6502::Implied},   // 0xC3 (Illegal)
		{"CPY", 2, 3, &CPU6502::CPY, &CPU6502::ZeroPage},  // 0xC4
		{"CMP", 2, 3, &CPU6502::CMP, &CPU6502::ZeroPage},  // 0xC5
		{"DEC", 2, 5, &CPU6502::DEC, &CPU6502::ZeroPage},  // 0xC6
		{"ILL", 1, 0, &CPU6502::ILL, &CPU6502::Implied},   // 0xC7 (Illegal)
		{"INY", 1, 2, &CPU6502::INY, &CPU6502::Implied},   // 0xC8
		{"CMP", 2, 2, &CPU6502::CMP, &CPU6502::Immediate}, // 0xC9
		{"DEX", 1, 2, &CPU6502::DEX, &CPU6502::Implied},   // 0xCA
		{"ILL", 1, 0, &CPU6502::ILL, &CPU6502::Implied},   // 0xCB (Illegal)
		{"CPY", 3, 4, &CPU6502::CPY, &CPU6502::Absolute},  // 0xCC
		{"CMP", 3, 4, &CPU6502::CMP, &CPU6502::Absolute},  // 0xCD
		{"DEC", 3, 6, &CPU6502::DEC, &CPU6502::Absolute},  // 0xCE
		{"ILL", 1, 0, &CPU6502::ILL, &CPU6502::Implied},   // 0xCF (Illegal)

		{"BNE", 2, 2, &CPU6502::BNE, &CPU6502::Relative},  // 0xD0
		{"CMP", 2, 5, &CPU6502::CMP, &CPU6502::IndirectY}, // 0xD1
		{"ILL", 1, 0, &CPU6502::ILL, &CPU6502::Implied},   // 0xD2 (Illegal)
		{"ILL", 1, 0, &CPU6502::ILL, &CPU6502::Implied},   // 0xD3 (Illegal)
		{"NOP", 2, 4, &CPU6502::NOP, &CPU6502::ZeroPageX}, // 0xD4 (Illegal, oft als NOP behandelt)
		{"CMP", 2, 4, &CPU6502::CMP, &CPU6502::ZeroPageX}, // 0xD5
		{"DEC", 2, 6, &CPU6502::DEC, &CPU6502::ZeroPageX}, // 0xD6
		{"ILL", 1, 0, &CPU6502::ILL, &CPU6502::Implied},   // 0xD7 (Illegal)
		{"CLD", 1, 2, &CPU6502::CLD, &CPU6502::Implied},   // 0xD8
		{"CMP", 3, 4, &CPU6502::CMP, &CPU6502::AbsoluteY}, // 0xD9
		{"NOP", 1, 2, &CPU6502::NOP, &CPU6502::Implied},   // 0xDA (Illegal, oft als NOP behandelt)
		{"ILL", 1, 0, &CPU6502::ILL, &CPU6502::Implied},   // 0xDB (Illegal)
		{"NOP", 3, 4, &CPU6502::NOP, &CPU6502::AbsoluteX}, // 0xDC (Illegal, oft als NOP behandelt)
		{"CMP", 3, 4, &CPU6502::CMP, &CPU6502::AbsoluteX}, // 0xDD
		{"DEC", 3, 7, &CPU6502::DEC, &CPU6502::AbsoluteX}, // 0xDE
		{"ILL", 1, 0, &CPU6502::ILL, &CPU6502::Implied},   // 0xDF (Illegal)

		{"CPX", 2, 2, &CPU6502::CPX, &CPU6502::Immediate}, // 0xE0
		{"SBC", 2, 6, &CPU6502::SBC, &CPU6502::IndirectX}, // 0xE1
		{"NOP", 2, 2, &CPU6502::NOP, &CPU6502::Immediate}, // 0xE2 (Illegal, oft als NOP behandelt)
		{"ILL", 1, 0, &CPU6502::ILL, &CPU6502::Implied},   // 0xE3 (Illegal)
		{"CPX", 2, 3, &CPU6502::CPX, &CPU6502::ZeroPage},  // 0xE4
		{"SBC", 2, 3, &CPU6502::SBC, &CPU6502::ZeroPage},  // 0xE5
		{"INC", 2, 5, &CPU6502::INC, &CPU6502::ZeroPage},  // 0xE6
		{"ILL", 1, 0, &CPU6502::ILL, &CPU6502::Implied},   // 0xE7 (Illegal)
		{"INX", 1, 2, &CPU6502::INX, &CPU6502::Implied},   // 0xE8
		{"SBC", 2, 2, &CPU6502::SBC, &CPU6502::Immediate}, // 0xE9
		{"NOP", 1, 2, &CPU6502::NOP, &CPU6502::Implied},   // 0xEA (NOP)
		{"ILL", 1, 0, &CPU6502::ILL, &CPU6502::Implied},   // 0xEB (Illegal)
		{"CPX", 3, 4, &CPU6502::CPX, &CPU6502::Absolute},  // 0xEC
		{"SBC", 3, 4, &CPU6502::SBC, &CPU6502::Absolute},  // 0xED
		{"INC", 3, 6, &CPU6502::INC, &CPU6502::Absolute},  // 0xEE
		{"ILL", 1, 0, &CPU6502::ILL, &CPU6502::Implied},   // 0xEF (Illegal)

		{"BEQ", 2, 2, &CPU6502::BEQ, &CPU6502::Relative},  // 0xF0
		{"SBC", 2, 5, &CPU6502::SBC, &CPU6502::IndirectY}, // 0xF1
		{"ILL", 1, 0, &CPU6502::ILL, &CPU6502::Implied},   // 0xF2 (Illegal)
		{"ILL", 1, 0, &CPU6502::ILL, &CPU6502::Implied},   // 0xF3 (Illegal)
		{"NOP", 2, 4, &CPU6502::NOP, &CPU6502::ZeroPageX}, // 0xF4 (Illegal, oft als NOP behandelt)
		{"SBC", 2, 4, &CPU6502::SBC, &CPU6502::ZeroPageX}, // 0xF5
		{"INC", 2, 6, &CPU6502::INC, &CPU6502::ZeroPageX}, // 0xF6
		{"ILL", 1, 0, &CPU6502::ILL, &CPU6502::Implied},   // 0xF7 (Illegal)
		{"SED", 1, 2, &CPU6502::SED, &CPU6502::Implied},   // 0xF8
		{"SBC", 3, 4, &CPU6502::SBC, &CPU6502::AbsoluteY}, // 0xF9
		{"NOP", 1, 2, &CPU6502::NOP, &CPU6502::Implied},   // 0xFA (Illegal, oft als NOP behandelt)
		{"ILL", 1, 0, &CPU6502::ILL, &CPU6502::Implied},   // 0xFB (Illegal)
		{"NOP", 3, 4, &CPU6502::NOP, &CPU6502::AbsoluteX}, // 0xFC (Illegal, oft als NOP behandelt)
		{"SBC", 3, 4, &CPU6502::SBC, &CPU6502::AbsoluteX}, // 0xFD
		{"INC", 3, 7, &CPU6502::INC, &CPU6502::AbsoluteX}, // 0xFE
		{"ILL", 1, 0, &CPU6502::ILL, &CPU6502::Implied}	   // 0xFF (Illegal)
	};

private:
	u16 Immediate();
	u16 ZeroPage();
	u16 ZeroPageX();
	u16 ZeroPageY();
	u16 Absolute();
	u16 AbsoluteX();
	u16 AbsoluteY();
	u16 Indirect();
	u16 IndirectX();
	u16 IndirectY();
	u16 Implied();
	u16 Accumulator();
	u16 Relative();

private:
	void ILL(); // Illegal
	void LDA(); // Load Accumulator
	void LDX(); // Load X Register
	void LDY(); // Load Y Register
	void STA(); // Store Accumulator
	void STX(); // Store X Register
	void STY(); // Store Y Register
	void TAX(); // Transfer accumulator to X
	void TAY(); // Transfer accumulator to Y
	void TXA(); // Transfer X to accumulator
	void TYA(); // Transfer Y to accumulator
	void TSX(); // Transfer stack pointer to X
	void TXS(); // Transfer X to stack pointer
	void PHA(); // Push accumulator on stack
	void PHP(); // Push processor status on stack
	void PLA(); // Pull accumulator from stack
	void PLP(); // Pull processor status from stack
	void AND(); // Logical AND
	void EOR(); // Exclusive OR
	void ORA(); // Logical Inclusive OR
	void BIT(); // Bit Test
	void ADC(); // Add with Carry
	void SBC(); // Subtract with Carry
	void CMP(); // Compare accumulator
	void CPX(); // Compare X register
	void CPY(); // Compare Y register
	void INC(); // Increment a memory location
	void INX(); // Increment the X register
	void INY(); // Increment the Y register
	void DEC(); // Decrement a memory location
	void DEX(); // Decrement the X register
	void DEY(); // Decrement the Y register
	void ASL(); // Arithmetic Shift Left
	void LSR(); // Logical Shift Right
	void ROL(); // Rotate Left
	void ROR(); // Rotate Right
	void JMP(); // Jump to another location
	void JSR(); // Jump to a subroutine
	void RTS(); // Return from subroutine
	void BCC(); // Branch if carry flag clear
	void BCS(); // Branch if carry flag set
	void BEQ(); // Branch if zero flag set
	void BMI(); // Branch if negative flag set
	void BNE(); // Branch if zero flag clear
	void BPL(); // Branch if negative flag clear
	void BVC(); // Branch if overflow flag clear
	void BVS(); // Branch if overflow flag set
	void CLC(); // Clear carry flag
	void CLD(); // Clear decimal mode flag
	void CLI(); // Clear interrupt disable flag
	void CLV(); // Clear overflow flag
	void SEC(); // Set carry flag
	void SED(); // Set decimal mode flag
	void SEI(); // Set interrupt disable flag
	void BRK(); // Force an interrupt
	void NOP(); // No Operation
	void RTI(); // Return from Interrupt
};