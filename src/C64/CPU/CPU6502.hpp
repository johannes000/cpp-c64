#pragma once
#include "Utility\Includes.hpp"
#include "C64\Component.hpp"
#include "C64\Memory\Memory.hpp"

class C64;
union ProcessorStatus {
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
};

struct CPUState {
	u8 A;
	u8 X;
	u8 Y;
	ProcessorStatus PS;
	u8 SP;
	u16 PC;
};

class CPU6502 : public Component {
public:
	struct Instruction {
		u8 OpCode;
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

	bool mIsJump;

	void RunOneInstruction();
	void LogCurrentInstruction();

	auto GetPC() const { return PC; }

	[[nodiscard]] Instruction GetInstructionAtAddr(u16 addr) const;
	[[nodiscard]] Instruction GetCurrentInstrution() const { return mCurrentInstruction; }
	[[nodiscard]] Instruction GetInstruction(u8 opCode) const { return OpCodes[opCode]; };
	std::string GetCurrentInstructionAddressString() const;
	const std::string GetCurrentInstructionLog() const;

	u8 IOLoad8(u16 address) const override;
	u16 IOLoad16(u16 address) const override;

	void IOStore8(u16 address, u8 value) override;
	void IOStore16(u16 address, u16 value) override;

	void Reset();

	CPUState GetCpuState() const {
		return mCpuStateSnapshot;
	}

private:
	void UpdateCpuStateSnapshot();
	void PushToStack(u8 val);
	void PushAddressToStack(u16 addr);

private:
	C64 *mC64;

	Instruction mCurrentInstruction;
	CPUState mCpuStateSnapshot{};

	// Accumulator
	u8 A;
	// Index Register X
	u8 X;
	// Index Register Y
	u8 Y;
	// Processor Status
	ProcessorStatus PS;
	// Stack Pointer
	u8 SP;
	// Program Counter
	u16 PC;

	std::shared_ptr<spdlog::logger> log;

private:
	const std::vector<Instruction> OpCodes{
		{0x00, "BRK", 1, 7, &CPU6502::BRK, &CPU6502::Implied},	   // 0x00
		{0x01, "ORA", 2, 6, &CPU6502::ORA, &CPU6502::IndirectX},   // 0x01
		{0x02, "ILL", 1, 0, &CPU6502::ILL, &CPU6502::Implied},	   // 0x02 (Illegal)
		{0x03, "ILL", 1, 0, &CPU6502::ILL, &CPU6502::Implied},	   // 0x03 (Illegal)
		{0x04, "NOP", 2, 3, &CPU6502::NOP, &CPU6502::ZeroPage},	   // 0x04 (Illegal, oft als NOP behandelt)
		{0x05, "ORA", 2, 3, &CPU6502::ORA, &CPU6502::ZeroPage},	   // 0x05
		{0x06, "ASL", 2, 5, &CPU6502::ASL, &CPU6502::ZeroPage},	   // 0x06
		{0x07, "ILL", 1, 0, &CPU6502::ILL, &CPU6502::Implied},	   // 0x07 (Illegal)
		{0x08, "PHP", 1, 3, &CPU6502::PHP, &CPU6502::Implied},	   // 0x08
		{0x09, "ORA", 2, 2, &CPU6502::ORA, &CPU6502::Immediate},   // 0x09
		{0x0A, "ASL", 1, 2, &CPU6502::ASL, &CPU6502::Accumulator}, // 0x0A
		{0x0B, "ILL", 1, 0, &CPU6502::ILL, &CPU6502::Implied},	   // 0x0B (Illegal)
		{0x0C, "NOP", 3, 4, &CPU6502::NOP, &CPU6502::Absolute},	   // 0x0C (Illegal, oft als NOP behandelt)
		{0x0D, "ORA", 3, 4, &CPU6502::ORA, &CPU6502::Absolute},	   // 0x0D
		{0x0E, "ASL", 3, 6, &CPU6502::ASL, &CPU6502::Absolute},	   // 0x0E
		{0x0F, "ILL", 1, 0, &CPU6502::ILL, &CPU6502::Implied},	   // 0x0F (Illegal)

		{0x10, "BPL", 2, 2, &CPU6502::BPL, &CPU6502::Relative},	 // 0x10
		{0x11, "ORA", 2, 5, &CPU6502::ORA, &CPU6502::IndirectY}, // 0x11
		{0x12, "ILL", 1, 0, &CPU6502::ILL, &CPU6502::Implied},	 // 0x12 (Illegal)
		{0x13, "ILL", 1, 0, &CPU6502::ILL, &CPU6502::Implied},	 // 0x13 (Illegal)
		{0x14, "NOP", 2, 4, &CPU6502::NOP, &CPU6502::ZeroPageX}, // 0x14 (Illegal, oft als NOP behandelt)
		{0x15, "ORA", 2, 4, &CPU6502::ORA, &CPU6502::ZeroPageX}, // 0x15
		{0x16, "ASL", 2, 6, &CPU6502::ASL, &CPU6502::ZeroPageX}, // 0x16
		{0x17, "ILL", 1, 0, &CPU6502::ILL, &CPU6502::Implied},	 // 0x17 (Illegal)
		{0x18, "CLC", 1, 2, &CPU6502::CLC, &CPU6502::Implied},	 // 0x18
		{0x19, "ORA", 3, 4, &CPU6502::ORA, &CPU6502::AbsoluteY}, // 0x19
		{0x1A, "NOP", 1, 2, &CPU6502::NOP, &CPU6502::Implied},	 // 0x1A (Illegal, oft als NOP behandelt)
		{0x1B, "ILL", 1, 0, &CPU6502::ILL, &CPU6502::Implied},	 // 0x1B (Illegal)
		{0x1C, "NOP", 3, 4, &CPU6502::NOP, &CPU6502::AbsoluteX}, // 0x1C (Illegal, oft als NOP behandelt)
		{0x1D, "ORA", 3, 4, &CPU6502::ORA, &CPU6502::AbsoluteX}, // 0x1D
		{0x1E, "ASL", 3, 7, &CPU6502::ASL, &CPU6502::AbsoluteX}, // 0x1E
		{0x1F, "ILL", 1, 0, &CPU6502::ILL, &CPU6502::Implied},	 // 0x1F (Illegal)

		{0x20, "JSR", 3, 6, &CPU6502::JSR, &CPU6502::Absolute},	   // 0x20
		{0x21, "AND", 2, 6, &CPU6502::AND, &CPU6502::IndirectX},   // 0x21
		{0x22, "ILL", 1, 0, &CPU6502::ILL, &CPU6502::Implied},	   // 0x22 (Illegal)
		{0x23, "ILL", 1, 0, &CPU6502::ILL, &CPU6502::Implied},	   // 0x23 (Illegal)
		{0x24, "BIT", 2, 3, &CPU6502::BIT, &CPU6502::ZeroPage},	   // 0x24
		{0x25, "AND", 2, 3, &CPU6502::AND, &CPU6502::ZeroPage},	   // 0x25
		{0x26, "ROL", 2, 5, &CPU6502::ROL, &CPU6502::ZeroPage},	   // 0x26
		{0x27, "ILL", 1, 0, &CPU6502::ILL, &CPU6502::Implied},	   // 0x27 (Illegal)
		{0x28, "PLP", 1, 4, &CPU6502::PLP, &CPU6502::Implied},	   // 0x28
		{0x29, "AND", 2, 2, &CPU6502::AND, &CPU6502::Immediate},   // 0x29
		{0x2A, "ROL", 1, 2, &CPU6502::ROL, &CPU6502::Accumulator}, // 0x2A
		{0x2B, "ILL", 1, 0, &CPU6502::ILL, &CPU6502::Implied},	   // 0x2B (Illegal)
		{0x2C, "BIT", 3, 4, &CPU6502::BIT, &CPU6502::Absolute},	   // 0x2C
		{0x2D, "AND", 3, 4, &CPU6502::AND, &CPU6502::Absolute},	   // 0x2D
		{0x2E, "ROL", 3, 6, &CPU6502::ROL, &CPU6502::Absolute},	   // 0x2E
		{0x2F, "ILL", 1, 0, &CPU6502::ILL, &CPU6502::Implied},	   // 0x2F (Illegal)

		{0x30, "BMI", 2, 2, &CPU6502::BMI, &CPU6502::Relative},	 // 0x30
		{0x31, "AND", 2, 5, &CPU6502::AND, &CPU6502::IndirectY}, // 0x31
		{0x32, "ILL", 1, 0, &CPU6502::ILL, &CPU6502::Implied},	 // 0x32 (Illegal)
		{0x33, "ILL", 1, 0, &CPU6502::ILL, &CPU6502::Implied},	 // 0x33 (Illegal)
		{0x34, "NOP", 2, 4, &CPU6502::NOP, &CPU6502::ZeroPageX}, // 0x34 (Illegal, oft als NOP behandelt)
		{0x35, "AND", 2, 4, &CPU6502::AND, &CPU6502::ZeroPageX}, // 0x35
		{0x36, "ROL", 2, 6, &CPU6502::ROL, &CPU6502::ZeroPageX}, // 0x36
		{0x37, "ILL", 1, 0, &CPU6502::ILL, &CPU6502::Implied},	 // 0x37 (Illegal)
		{0x38, "SEC", 1, 2, &CPU6502::SEC, &CPU6502::Implied},	 // 0x38
		{0x39, "AND", 3, 4, &CPU6502::AND, &CPU6502::AbsoluteY}, // 0x39
		{0x3A, "NOP", 1, 2, &CPU6502::NOP, &CPU6502::Implied},	 // 0x3A (Illegal, oft als NOP behandelt)
		{0x3B, "ILL", 1, 0, &CPU6502::ILL, &CPU6502::Implied},	 // 0x3B (Illegal)
		{0x3C, "NOP", 3, 4, &CPU6502::NOP, &CPU6502::AbsoluteX}, // 0x3C (Illegal, oft als NOP behandelt)
		{0x3D, "AND", 3, 4, &CPU6502::AND, &CPU6502::AbsoluteX}, // 0x3D
		{0x3E, "ROL", 3, 7, &CPU6502::ROL, &CPU6502::AbsoluteX}, // 0x3E
		{0x3F, "ILL", 1, 0, &CPU6502::ILL, &CPU6502::Implied},	 // 0x3F (Illegal)

		{0x40, "RTI", 1, 6, &CPU6502::RTI, &CPU6502::Implied},	   // 0x40
		{0x41, "EOR", 2, 6, &CPU6502::EOR, &CPU6502::IndirectX},   // 0x41
		{0x42, "ILL", 1, 0, &CPU6502::ILL, &CPU6502::Implied},	   // 0x42 (Illegal)
		{0x43, "ILL", 1, 0, &CPU6502::ILL, &CPU6502::Implied},	   // 0x43 (Illegal)
		{0x44, "NOP", 2, 3, &CPU6502::NOP, &CPU6502::ZeroPage},	   // 0x44 (Illegal, oft als NOP behandelt)
		{0x45, "EOR", 2, 3, &CPU6502::EOR, &CPU6502::ZeroPage},	   // 0x45
		{0x46, "LSR", 2, 5, &CPU6502::LSR, &CPU6502::ZeroPage},	   // 0x46
		{0x47, "ILL", 1, 0, &CPU6502::ILL, &CPU6502::Implied},	   // 0x47 (Illegal)
		{0x48, "PHA", 1, 3, &CPU6502::PHA, &CPU6502::Implied},	   // 0x48
		{0x49, "EOR", 2, 2, &CPU6502::EOR, &CPU6502::Immediate},   // 0x49
		{0x4A, "LSR", 1, 2, &CPU6502::LSR, &CPU6502::Accumulator}, // 0x4A
		{0x4B, "ILL", 1, 0, &CPU6502::ILL, &CPU6502::Implied},	   // 0x4B (Illegal)
		{0x4C, "JMP", 3, 3, &CPU6502::JMP, &CPU6502::Absolute},	   // 0x4C
		{0x4D, "EOR", 3, 4, &CPU6502::EOR, &CPU6502::Absolute},	   // 0x4D
		{0x4E, "LSR", 3, 6, &CPU6502::LSR, &CPU6502::Absolute},	   // 0x4E
		{0x4F, "ILL", 1, 0, &CPU6502::ILL, &CPU6502::Implied},	   // 0x4F (Illegal)

		{0x50, "BVC", 2, 2, &CPU6502::BVC, &CPU6502::Relative},	 // 0x50
		{0x51, "EOR", 2, 5, &CPU6502::EOR, &CPU6502::IndirectY}, // 0x51
		{0x52, "ILL", 1, 0, &CPU6502::ILL, &CPU6502::Implied},	 // 0x52 (Illegal)
		{0x53, "ILL", 1, 0, &CPU6502::ILL, &CPU6502::Implied},	 // 0x53 (Illegal)
		{0x54, "NOP", 2, 4, &CPU6502::NOP, &CPU6502::ZeroPageX}, // 0x54 (Illegal, oft als NOP behandelt)
		{0x55, "EOR", 2, 4, &CPU6502::EOR, &CPU6502::ZeroPageX}, // 0x55
		{0x56, "LSR", 2, 6, &CPU6502::LSR, &CPU6502::ZeroPageX}, // 0x56
		{0x57, "ILL", 1, 0, &CPU6502::ILL, &CPU6502::Implied},	 // 0x57 (Illegal)
		{0x58, "CLI", 1, 2, &CPU6502::CLI, &CPU6502::Implied},	 // 0x58
		{0x59, "EOR", 3, 4, &CPU6502::EOR, &CPU6502::AbsoluteY}, // 0x59
		{0x5A, "NOP", 1, 2, &CPU6502::NOP, &CPU6502::Implied},	 // 0x5A (Illegal, oft als NOP behandelt)
		{0x5B, "ILL", 1, 0, &CPU6502::ILL, &CPU6502::Implied},	 // 0x5B (Illegal)
		{0x5C, "NOP", 3, 4, &CPU6502::NOP, &CPU6502::AbsoluteX}, // 0x5C (Illegal, oft als NOP behandelt)
		{0x5D, "EOR", 3, 4, &CPU6502::EOR, &CPU6502::AbsoluteX}, // 0x5D
		{0x5E, "LSR", 3, 7, &CPU6502::LSR, &CPU6502::AbsoluteX}, // 0x5E
		{0x5F, "ILL", 1, 0, &CPU6502::ILL, &CPU6502::Implied},	 // 0x5F (Illegal)

		{0x60, "RTS", 1, 6, &CPU6502::RTS, &CPU6502::Implied},	   // 0x60
		{0x61, "ADC", 2, 6, &CPU6502::ADC, &CPU6502::IndirectX},   // 0x61
		{0x62, "ILL", 1, 0, &CPU6502::ILL, &CPU6502::Implied},	   // 0x62 (Illegal)
		{0x63, "ILL", 1, 0, &CPU6502::ILL, &CPU6502::Implied},	   // 0x63 (Illegal)
		{0x64, "NOP", 2, 3, &CPU6502::NOP, &CPU6502::ZeroPage},	   // 0x64 (Illegal, oft als NOP behandelt)
		{0x65, "ADC", 2, 3, &CPU6502::ADC, &CPU6502::ZeroPage},	   // 0x65
		{0x66, "ROR", 2, 5, &CPU6502::ROR, &CPU6502::ZeroPage},	   // 0x66
		{0x67, "ILL", 1, 0, &CPU6502::ILL, &CPU6502::Implied},	   // 0x67 (Illegal)
		{0x68, "PLA", 1, 4, &CPU6502::PLA, &CPU6502::Implied},	   // 0x68
		{0x69, "ADC", 2, 2, &CPU6502::ADC, &CPU6502::Immediate},   // 0x69
		{0x6A, "ROR", 1, 2, &CPU6502::ROR, &CPU6502::Accumulator}, // 0x6A
		{0x6B, "ILL", 1, 0, &CPU6502::ILL, &CPU6502::Implied},	   // 0x6B (Illegal)
		{0x6C, "JMP", 3, 5, &CPU6502::JMP, &CPU6502::Indirect},	   // 0x6C
		{0x6D, "ADC", 3, 4, &CPU6502::ADC, &CPU6502::Absolute},	   // 0x6D
		{0x6E, "ROR", 3, 6, &CPU6502::ROR, &CPU6502::Absolute},	   // 0x6E
		{0x6F, "ILL", 1, 0, &CPU6502::ILL, &CPU6502::Implied},	   // 0x6F (Illegal)

		{0x70, "BVS", 2, 2, &CPU6502::BVS, &CPU6502::Relative},	 // 0x70
		{0x71, "ADC", 2, 5, &CPU6502::ADC, &CPU6502::IndirectY}, // 0x71
		{0x72, "ILL", 1, 0, &CPU6502::ILL, &CPU6502::Implied},	 // 0x72 (Illegal)
		{0x73, "ILL", 1, 0, &CPU6502::ILL, &CPU6502::Implied},	 // 0x73 (Illegal)
		{0x74, "NOP", 2, 4, &CPU6502::NOP, &CPU6502::ZeroPageX}, // 0x74 (Illegal, oft als NOP behandelt)
		{0x75, "ADC", 2, 4, &CPU6502::ADC, &CPU6502::ZeroPageX}, // 0x75
		{0x76, "ROR", 2, 6, &CPU6502::ROR, &CPU6502::ZeroPageX}, // 0x76
		{0x77, "ILL", 1, 0, &CPU6502::ILL, &CPU6502::Implied},	 // 0x77 (Illegal)
		{0x78, "SEI", 1, 2, &CPU6502::SEI, &CPU6502::Implied},	 // 0x78
		{0x79, "ADC", 3, 4, &CPU6502::ADC, &CPU6502::AbsoluteY}, // 0x79
		{0x7A, "NOP", 1, 2, &CPU6502::NOP, &CPU6502::Implied},	 // 0x7A (Illegal, oft als NOP behandelt)
		{0x7B, "ILL", 1, 0, &CPU6502::ILL, &CPU6502::Implied},	 // 0x7B (Illegal)
		{0x7C, "NOP", 3, 4, &CPU6502::NOP, &CPU6502::AbsoluteX}, // 0x7C (Illegal, oft als NOP behandelt)
		{0x7D, "ADC", 3, 4, &CPU6502::ADC, &CPU6502::AbsoluteX}, // 0x7D
		{0x7E, "ROR", 3, 7, &CPU6502::ROR, &CPU6502::AbsoluteX}, // 0x7E
		{0x7F, "ILL", 1, 0, &CPU6502::ILL, &CPU6502::Implied},	 // 0x7F (Illegal)

		{0x80, "NOP", 2, 2, &CPU6502::NOP, &CPU6502::Immediate}, // 0x80 (Illegal, oft als NOP behandelt)
		{0x81, "STA", 2, 6, &CPU6502::STA, &CPU6502::IndirectX}, // 0x81
		{0x82, "NOP", 2, 2, &CPU6502::NOP, &CPU6502::Immediate}, // 0x82 (Illegal, oft als NOP behandelt)
		{0x83, "ILL", 1, 0, &CPU6502::ILL, &CPU6502::Implied},	 // 0x83 (Illegal)
		{0x84, "STY", 2, 3, &CPU6502::STY, &CPU6502::ZeroPage},	 // 0x84
		{0x85, "STA", 2, 3, &CPU6502::STA, &CPU6502::ZeroPage},	 // 0x85
		{0x86, "STX", 2, 3, &CPU6502::STX, &CPU6502::ZeroPage},	 // 0x86
		{0x87, "ILL", 1, 0, &CPU6502::ILL, &CPU6502::Implied},	 // 0x87 (Illegal)
		{0x88, "DEY", 1, 2, &CPU6502::DEY, &CPU6502::Implied},	 // 0x88
		{0x89, "NOP", 2, 2, &CPU6502::NOP, &CPU6502::Immediate}, // 0x89 (Illegal, oft als NOP behandelt)
		{0x8A, "TXA", 1, 2, &CPU6502::TXA, &CPU6502::Implied},	 // 0x8A
		{0x8B, "ILL", 1, 0, &CPU6502::ILL, &CPU6502::Implied},	 // 0x8B (Illegal)
		{0x8C, "STY", 3, 4, &CPU6502::STY, &CPU6502::Absolute},	 // 0x8C
		{0x8D, "STA", 3, 4, &CPU6502::STA, &CPU6502::Absolute},	 // 0x8D
		{0x8E, "STX", 3, 4, &CPU6502::STX, &CPU6502::Absolute},	 // 0x8E
		{0x8F, "ILL", 1, 0, &CPU6502::ILL, &CPU6502::Implied},	 // 0x8F (Illegal)

		{0x90, "BCC", 2, 2, &CPU6502::BCC, &CPU6502::Relative},	 // 0x90
		{0x91, "STA", 2, 6, &CPU6502::STA, &CPU6502::IndirectY}, // 0x91
		{0x92, "ILL", 1, 0, &CPU6502::ILL, &CPU6502::Implied},	 // 0x92 (Illegal)
		{0x93, "ILL", 1, 0, &CPU6502::ILL, &CPU6502::Implied},	 // 0x93 (Illegal)
		{0x94, "STY", 2, 4, &CPU6502::STY, &CPU6502::ZeroPageX}, // 0x94
		{0x95, "STA", 2, 4, &CPU6502::STA, &CPU6502::ZeroPageX}, // 0x95
		{0x96, "STX", 2, 4, &CPU6502::STX, &CPU6502::ZeroPageY}, // 0x96
		{0x97, "ILL", 1, 0, &CPU6502::ILL, &CPU6502::Implied},	 // 0x97 (Illegal)
		{0x98, "TYA", 1, 2, &CPU6502::TYA, &CPU6502::Implied},	 // 0x98
		{0x99, "STA", 3, 5, &CPU6502::STA, &CPU6502::AbsoluteY}, // 0x99
		{0x9A, "TXS", 1, 2, &CPU6502::TXS, &CPU6502::Implied},	 // 0x9A
		{0x9B, "ILL", 1, 0, &CPU6502::ILL, &CPU6502::Implied},	 // 0x9B (Illegal)
		{0x9C, "ILL", 1, 0, &CPU6502::ILL, &CPU6502::Implied},	 // 0x9C (Illegal)
		{0x9D, "STA", 3, 5, &CPU6502::STA, &CPU6502::AbsoluteX}, // 0x9D
		{0x9E, "ILL", 1, 0, &CPU6502::ILL, &CPU6502::Implied},	 // 0x9E (Illegal)
		{0x9F, "ILL", 1, 0, &CPU6502::ILL, &CPU6502::Implied},	 // 0x9F (Illegal)

		{0xA0, "LDY", 2, 2, &CPU6502::LDY, &CPU6502::Immediate}, // 0xA0
		{0xA1, "LDA", 2, 6, &CPU6502::LDA, &CPU6502::IndirectX}, // 0xA1
		{0xA2, "LDX", 2, 2, &CPU6502::LDX, &CPU6502::Immediate}, // 0xA2
		{0xA3, "ILL", 1, 0, &CPU6502::ILL, &CPU6502::Implied},	 // 0xA3 (Illegal)
		{0xA4, "LDY", 2, 3, &CPU6502::LDY, &CPU6502::ZeroPage},	 // 0xA4
		{0xA5, "LDA", 2, 3, &CPU6502::LDA, &CPU6502::ZeroPage},	 // 0xA5
		{0xA6, "LDX", 2, 3, &CPU6502::LDX, &CPU6502::ZeroPage},	 // 0xA6
		{0xA7, "ILL", 1, 0, &CPU6502::ILL, &CPU6502::Implied},	 // 0xA7 (Illegal)
		{0xA8, "TAY", 1, 2, &CPU6502::TAY, &CPU6502::Implied},	 // 0xA8
		{0xA9, "LDA", 2, 2, &CPU6502::LDA, &CPU6502::Immediate}, // 0xA9
		{0xAA, "TAX", 1, 2, &CPU6502::TAX, &CPU6502::Implied},	 // 0xAA
		{0xAB, "ILL", 1, 0, &CPU6502::ILL, &CPU6502::Implied},	 // 0xAB (Illegal)
		{0xAC, "LDY", 3, 4, &CPU6502::LDY, &CPU6502::Absolute},	 // 0xAC
		{0xAD, "LDA", 3, 4, &CPU6502::LDA, &CPU6502::Absolute},	 // 0xAD
		{0xAE, "LDX", 3, 4, &CPU6502::LDX, &CPU6502::Absolute},	 // 0xAE
		{0xAF, "ILL", 1, 0, &CPU6502::ILL, &CPU6502::Implied},	 // 0xAF (Illegal)

		{0xB0, "BCS", 2, 2, &CPU6502::BCS, &CPU6502::Relative},	 // 0xB0
		{0xB1, "LDA", 2, 5, &CPU6502::LDA, &CPU6502::IndirectY}, // 0xB1
		{0xB2, "ILL", 1, 0, &CPU6502::ILL, &CPU6502::Implied},	 // 0xB2 (Illegal)
		{0xB3, "ILL", 1, 0, &CPU6502::ILL, &CPU6502::Implied},	 // 0xB3 (Illegal)
		{0xB4, "LDY", 2, 4, &CPU6502::LDY, &CPU6502::ZeroPageX}, // 0xB4
		{0xB5, "LDA", 2, 4, &CPU6502::LDA, &CPU6502::ZeroPageX}, // 0xB5
		{0xB6, "LDX", 2, 4, &CPU6502::LDX, &CPU6502::ZeroPageY}, // 0xB6
		{0xB7, "ILL", 1, 0, &CPU6502::ILL, &CPU6502::Implied},	 // 0xB7 (Illegal)
		{0xB8, "CLV", 1, 2, &CPU6502::CLV, &CPU6502::Implied},	 // 0xB8
		{0xB9, "LDA", 3, 4, &CPU6502::LDA, &CPU6502::AbsoluteY}, // 0xB9
		{0xBA, "TSX", 1, 2, &CPU6502::TSX, &CPU6502::Implied},	 // 0xBA
		{0xBB, "ILL", 1, 0, &CPU6502::ILL, &CPU6502::Implied},	 // 0xBB (Illegal)
		{0xBC, "LDY", 3, 4, &CPU6502::LDY, &CPU6502::AbsoluteX}, // 0xBC
		{0xBD, "LDA", 3, 4, &CPU6502::LDA, &CPU6502::AbsoluteX}, // 0xBD
		{0xBE, "LDX", 3, 4, &CPU6502::LDX, &CPU6502::AbsoluteY}, // 0xBE
		{0xBF, "ILL", 1, 0, &CPU6502::ILL, &CPU6502::Implied},	 // 0xBF (Illegal)

		{0xC0, "CPY", 2, 2, &CPU6502::CPY, &CPU6502::Immediate}, // 0xC0
		{0xC1, "CMP", 2, 6, &CPU6502::CMP, &CPU6502::IndirectX}, // 0xC1
		{0xC2, "NOP", 2, 2, &CPU6502::NOP, &CPU6502::Immediate}, // 0xC2 (Illegal, oft als NOP behandelt)
		{0xC3, "ILL", 1, 0, &CPU6502::ILL, &CPU6502::Implied},	 // 0xC3 (Illegal)
		{0xC4, "CPY", 2, 3, &CPU6502::CPY, &CPU6502::ZeroPage},	 // 0xC4
		{0xC5, "CMP", 2, 3, &CPU6502::CMP, &CPU6502::ZeroPage},	 // 0xC5
		{0xC6, "DEC", 2, 5, &CPU6502::DEC, &CPU6502::ZeroPage},	 // 0xC6
		{0xC7, "ILL", 1, 0, &CPU6502::ILL, &CPU6502::Implied},	 // 0xC7 (Illegal)
		{0xC8, "INY", 1, 2, &CPU6502::INY, &CPU6502::Implied},	 // 0xC8
		{0xC9, "CMP", 2, 2, &CPU6502::CMP, &CPU6502::Immediate}, // 0xC9
		{0xCA, "DEX", 1, 2, &CPU6502::DEX, &CPU6502::Implied},	 // 0xCA
		{0xCB, "ILL", 1, 0, &CPU6502::ILL, &CPU6502::Implied},	 // 0xCB (Illegal)
		{0xCC, "CPY", 3, 4, &CPU6502::CPY, &CPU6502::Absolute},	 // 0xCC
		{0xCD, "CMP", 3, 4, &CPU6502::CMP, &CPU6502::Absolute},	 // 0xCD
		{0xCE, "DEC", 3, 6, &CPU6502::DEC, &CPU6502::Absolute},	 // 0xCE
		{0xCF, "ILL", 1, 0, &CPU6502::ILL, &CPU6502::Implied},	 // 0xCF (Illegal)

		{0xD0, "BNE", 2, 2, &CPU6502::BNE, &CPU6502::Relative},	 // 0xD0
		{0xD1, "CMP", 2, 5, &CPU6502::CMP, &CPU6502::IndirectY}, // 0xD1
		{0xD2, "ILL", 1, 0, &CPU6502::ILL, &CPU6502::Implied},	 // 0xD2 (Illegal)
		{0xD3, "ILL", 1, 0, &CPU6502::ILL, &CPU6502::Implied},	 // 0xD3 (Illegal)
		{0xD4, "NOP", 2, 4, &CPU6502::NOP, &CPU6502::ZeroPageX}, // 0xD4 (Illegal, oft als NOP behandelt)
		{0xD5, "CMP", 2, 4, &CPU6502::CMP, &CPU6502::ZeroPageX}, // 0xD5
		{0xD6, "DEC", 2, 6, &CPU6502::DEC, &CPU6502::ZeroPageX}, // 0xD6
		{0xD7, "ILL", 1, 0, &CPU6502::ILL, &CPU6502::Implied},	 // 0xD7 (Illegal)
		{0xD8, "CLD", 1, 2, &CPU6502::CLD, &CPU6502::Implied},	 // 0xD8
		{0xD9, "CMP", 3, 4, &CPU6502::CMP, &CPU6502::AbsoluteY}, // 0xD9
		{0xDA, "NOP", 1, 2, &CPU6502::NOP, &CPU6502::Implied},	 // 0xDA (Illegal, oft als NOP behandelt)
		{0xDB, "ILL", 1, 0, &CPU6502::ILL, &CPU6502::Implied},	 // 0xDB (Illegal)
		{0xDC, "NOP", 3, 4, &CPU6502::NOP, &CPU6502::AbsoluteX}, // 0xDC (Illegal, oft als NOP behandelt)
		{0xDD, "CMP", 3, 4, &CPU6502::CMP, &CPU6502::AbsoluteX}, // 0xDD
		{0xDE, "DEC", 3, 7, &CPU6502::DEC, &CPU6502::AbsoluteX}, // 0xDE
		{0xDF, "ILL", 1, 0, &CPU6502::ILL, &CPU6502::Implied},	 // 0xDF (Illegal)

		{0xE0, "CPX", 2, 2, &CPU6502::CPX, &CPU6502::Immediate}, // 0xE0
		{0xE1, "SBC", 2, 6, &CPU6502::SBC, &CPU6502::IndirectX}, // 0xE1
		{0xE2, "NOP", 2, 2, &CPU6502::NOP, &CPU6502::Immediate}, // 0xE2 (Illegal, oft als NOP behandelt)
		{0xE3, "ILL", 1, 0, &CPU6502::ILL, &CPU6502::Implied},	 // 0xE3 (Illegal)
		{0xE4, "CPX", 2, 3, &CPU6502::CPX, &CPU6502::ZeroPage},	 // 0xE4
		{0xE5, "SBC", 2, 3, &CPU6502::SBC, &CPU6502::ZeroPage},	 // 0xE5
		{0xE6, "INC", 2, 5, &CPU6502::INC, &CPU6502::ZeroPage},	 // 0xE6
		{0xE7, "ILL", 1, 0, &CPU6502::ILL, &CPU6502::Implied},	 // 0xE7 (Illegal)
		{0xE8, "INX", 1, 2, &CPU6502::INX, &CPU6502::Implied},	 // 0xE8
		{0xE9, "SBC", 2, 2, &CPU6502::SBC, &CPU6502::Immediate}, // 0xE9
		{0xEA, "NOP", 1, 2, &CPU6502::NOP, &CPU6502::Implied},	 // 0xEA (NOP)
		{0xEB, "ILL", 1, 0, &CPU6502::ILL, &CPU6502::Implied},	 // 0xEB (Illegal)
		{0xEC, "CPX", 3, 4, &CPU6502::CPX, &CPU6502::Absolute},	 // 0xEC
		{0xED, "SBC", 3, 4, &CPU6502::SBC, &CPU6502::Absolute},	 // 0xED
		{0xEE, "INC", 3, 6, &CPU6502::INC, &CPU6502::Absolute},	 // 0xEE
		{0xEF, "ILL", 1, 0, &CPU6502::ILL, &CPU6502::Implied},	 // 0xEF (Illegal)

		{0xF0, "BEQ", 2, 2, &CPU6502::BEQ, &CPU6502::Relative},	 // 0xF0
		{0xF1, "SBC", 2, 5, &CPU6502::SBC, &CPU6502::IndirectY}, // 0xF1
		{0xF2, "ILL", 1, 0, &CPU6502::ILL, &CPU6502::Implied},	 // 0xF2 (Illegal)
		{0xF3, "ILL", 1, 0, &CPU6502::ILL, &CPU6502::Implied},	 // 0xF3 (Illegal)
		{0xF4, "NOP", 2, 4, &CPU6502::NOP, &CPU6502::ZeroPageX}, // 0xF4 (Illegal, oft als NOP behandelt)
		{0xF5, "SBC", 2, 4, &CPU6502::SBC, &CPU6502::ZeroPageX}, // 0xF5
		{0xF6, "INC", 2, 6, &CPU6502::INC, &CPU6502::ZeroPageX}, // 0xF6
		{0xF7, "ILL", 1, 0, &CPU6502::ILL, &CPU6502::Implied},	 // 0xF7 (Illegal)
		{0xF8, "SED", 1, 2, &CPU6502::SED, &CPU6502::Implied},	 // 0xF8
		{0xF9, "SBC", 3, 4, &CPU6502::SBC, &CPU6502::AbsoluteY}, // 0xF9
		{0xFA, "NOP", 1, 2, &CPU6502::NOP, &CPU6502::Implied},	 // 0xFA (Illegal, oft als NOP behandelt)
		{0xFB, "ILL", 1, 0, &CPU6502::ILL, &CPU6502::Implied},	 // 0xFB (Illegal)
		{0xFC, "NOP", 3, 4, &CPU6502::NOP, &CPU6502::AbsoluteX}, // 0xFC (Illegal, oft als NOP behandelt)
		{0xFD, "SBC", 3, 4, &CPU6502::SBC, &CPU6502::AbsoluteX}, // 0xFD
		{0xFE, "INC", 3, 7, &CPU6502::INC, &CPU6502::AbsoluteX}, // 0xFE
		{0xFF, "ILL", 1, 0, &CPU6502::ILL, &CPU6502::Implied}	 // 0xFF (Illegal)
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