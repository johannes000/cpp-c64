#pragma once
#include "Utility\Includes.hpp"
#include "C64\Component.hpp"
#include "C64\Memory\Memory.hpp"

class CPU6502 : public Component {
public:
	enum struct AddressMode {
		Immediate,
		ZeroPage,
		ZeroPageX,
		Absolute,
		AbsoluteX,
		AbsoluteY,
		IndirectX,
		IndirectY
	};

	struct Instruction {
		u8 OpCode{0};
		AddressMode addressMode{0};
	};

public:
	CPU6502();
	~CPU6502();

private:
	Memory<kib(8)> mKernal;
	Memory<kib(8)> mBasic;
	Memory<kib(4)> mCharSet;

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
			u8 CF : 1;	   // Carry Flag: 1 on unsigned overflow
			u8 ZF : 1;	   // Zero Flag: 1 when all bits of a result are 0
			u8 IF : 1;	   // Interrupt Disable Flag: when 1, no interrupt will occur (except BRK and NMI)
			u8 DF : 1;	   // Decimal Mode Flag: 1 when CPU is in Decimal Mode
			u8 BF : 1;	   // Break Flag: 1 when pushed by instructions (BRK / PHP) and 0 when pushed by interrupts (NMI / IRQ)
			u8 unused : 1; // Unused: always set to 1
			u8 VF : 1;	   // Overflow Flag: 1 on signed overflow
			u8 NF : 1;	   // Negative Flag: 1 when result is negative
		};
	} PS;
	// Stack Pointer
	u8 SP;
	// Program Counter
	u8 PC;

	std::shared_ptr<spdlog::logger> log;

private:
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