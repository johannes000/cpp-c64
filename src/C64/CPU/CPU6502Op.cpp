#include "CPU6502.hpp"
#include "C64\C64.hpp"

void CPU6502::ILL() {
	log->warn("Illegale Instruction");
}

void CPU6502::LDA() {
	// 	Load Accumulator with Memory
	// M -> A
	A = GetValAtCurrentAddr();
	HandleNZFlags(A);
}

void CPU6502::LDX() {
	// 	Load Index X with Memory
	// M -> X
	X = GetValAtCurrentAddr();
	HandleNZFlags(X);
}

void CPU6502::LDY() {
	// Load Index Y with Memory
	// M -> Y
	Y = GetValAtCurrentAddr();
	HandleNZFlags(Y);
}

void CPU6502::STA() {
	// Store Accumulator in Memory
	// A -> M
	A = GetValAtCurrentAddr();
}

void CPU6502::STX() {
	// Store Index X in Memory
	// X->M
	X = GetValAtCurrentAddr();
}

void CPU6502::STY() {
	// Sore Index Y in Memory
	// Y->M
	Y = GetValAtCurrentAddr();
}

void CPU6502::TAX() {
	// Transfer Accumulator to Index X
	// A->X
	X = A;
	HandleNZFlags(A);
}

void CPU6502::TAY() {
	// Transfer Accumulator to Index Y
	// A -> Y
	Y = A;
	HandleNZFlags(A);
}

void CPU6502::TXA() {
	// Transfer Index X to Accumulator
	// X -> A
	A = X;
	HandleNZFlags(A);
}

void CPU6502::TYA() {
	// Transfer Index Y to Accumulator
	// Y -> A
	A = Y;
	HandleNZFlags(A);
}

void CPU6502::TSX() {
	// Transfer Index X to Stack Register
	// X -> SP
	X = SP;
	HandleNZFlags(X);
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
	HandleNZFlags(A);
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
	A &= GetValAtCurrentAddr();
	HandleNZFlags(A);
}

void CPU6502::EOR() {
	// Exclusive-OR Memory with Accumulator
	// A EOR M -> A
	A ^= GetValAtCurrentAddr();
	HandleNZFlags(A);
}

void CPU6502::ORA() {
	// OR Memory with Accumulator
	// A OR M -> A
	A |= GetValAtCurrentAddr();
	HandleNZFlags(A);
}

void CPU6502::BIT() {
	// Read the operand and set the N and V flags from the value (from the highest and next-to-highest bit respectively). Then take the operand, and AND it with the accumulator and evaluate the Z-flag.
	// N <= Operand(7)
	// V <= Operand(6)
	// Tmp <= Operand & AC
	// Z <= Z(Tmp)
	auto val = GetValAtCurrentAddr();
	PS.N = CheckBit(val, 7);
	PS.V = CheckBit(val, 6);
	auto temp = val & A;
	PS.Z = temp == 0;
}

void CPU6502::ADC() {
	// Add Memory to Accumulator with Carry
	// A + M + C -> A, C
	// TODO: BCD
	u8 val = GetValAtCurrentAddr();
	u16 temp = A + val + PS.C;
	if (temp > 0xFF)
		PS.C = 1;
	PS.V = (i16)temp < -128 || (i16)temp > 128;
	A = (u8)(temp & 0xFF);
	HandleNZFlags(A);
}

void CPU6502::SBC() {
	// Subtract with Carry
	// A,Z,C,N = A-M-(1-C)
	// This instruction subtracts the contents of a memory location to the accumulator
	// together with the not of the carry bit. If overflow occurs the carry bit is clear,
	// this enables multiple byte subtraction to be performed.
	// TODO: BCD
	u8 val = ~GetValAtCurrentAddr();
	u16 temp = A + val + PS.C;
	if (temp > 0xFF)
		PS.C = 1;
	PS.V = (i16)temp < -128 || (i16)temp > 128;
	A = (u8)(temp & 0xFF);
	HandleNZFlags(A);
}

void CPU6502::CMP() {
	// Z,C,N = A-M
	// This instruction compares the contents of the accumulator with another memory held value and sets the zero and carry flags as appropriate.
	// Carry Flag 	Set if A >= M
	// TODO: + add 1 cycle if page boundary crossed
	u8 val = GetValAtCurrentAddr();
	u8 temp = A - val;
	PS.C = A >= val;
	HandleNZFlags(temp);
}

void CPU6502::CPX() {
	// Z,C,N = X-M
	// This instruction compares the contents of the X register with another memory held value and sets the zero and carry flags as appropriate.
	u8 val = GetValAtCurrentAddr();
	u8 temp = X - val;
	PS.C = X >= val;
	HandleNZFlags(temp);
}

void CPU6502::CPY() {
	u8 val = GetValAtCurrentAddr();
	u8 temp = Y - val;
	PS.C = Y >= val;
	HandleNZFlags(temp);
}

void CPU6502::INC() {
	// Increment Memory
	// M,Z,N = M+1
	// Adds one to the value held at a specified memory location setting the zero and negative flags as appropriate.
	u8 res = GetValAtCurrentAddr() + 1;
	HandleNZFlags(res);
	StoreValAtCurrentAddr(res);
}

void CPU6502::INX() {
	// X,Z,N = X+1
	// Adds one to the X register setting the zero and negative flags as appropriate.
	++X;
	HandleNZFlags(X);
}

void CPU6502::INY() {
	// Y,Z,N = Y+1
	// Adds one to the Y register setting the zero and negative flags as appropriate.
	++Y;
	HandleNZFlags(Y);
}

void CPU6502::DEC() {
	// Wie Inc
	u8 res = GetValAtCurrentAddr() - 1;
	HandleNZFlags(res);
	StoreValAtCurrentAddr(res);
}

void CPU6502::DEX() {
	// Wie Inc
	--X;
	HandleNZFlags(X);
}

void CPU6502::DEY() {
	// Wie Inc
	--Y;
	HandleNZFlags(Y);
}

void CPU6502::ASL() {
	// Arithmetic Shift Left
	// A,Z,C,N = M*2 or M,Z,C,N = M*2
	// This operation shifts all the bits of the accumulator or memory contents one bit left.
	// Bit 0 is set to 0 and bit 7 is placed in the carry flag.
	// The effect of this operation is to multiply the memory contents by 2 (ignoring 2's complement considerations),
	// setting the carry if the result will not fit in 8 bits.
	bool isA = mCurrentInstruction.AddrFunc == &CPU6502::Accumulator;
	u16 val = isA ? A : GetValAtCurrentAddr();
	val <<= 1;
	PS.C = CheckBit(val, 8);
	HandleNZFlags(val);
	if (isA)
		A = val;
	else
		StoreValAtCurrentAddr(val);
}

void CPU6502::LSR() {
	// Logical shift right
	// A,C,Z,N = A/2 or M,C,Z,N = M/2
	// Each of the bits in A or M is shift one place to the right. The bit that was in bit 0 is shifted into the carry flag. Bit 7 is set to zero.
	bool isA = mCurrentInstruction.AddrFunc == &CPU6502::Accumulator;
	u8 val = isA ? A : GetValAtCurrentAddr();
	PS.C = CheckBit(val, 0);
	val >>= 1;
	HandleNZFlags(val);
	if (isA)
		A = val;
	else
		StoreValAtCurrentAddr(val);
}

void CPU6502::ROL() {
	// Rotate Left
	// Move each of the bits in either A or M one place to the left.
	// Bit 0 is filled with the current value of the carry flag whilst the old bit 7 becomes the new carry flag value.
	bool isA = mCurrentInstruction.AddrFunc == &CPU6502::Accumulator;
	u8 val = isA ? A : GetValAtCurrentAddr();
	u8 tmp = val;
	val <<= 1;
	SetBitTo(val, 0, PS.C);
	PS.C = CheckBit(tmp, 7);
	HandleNZFlags(val);
	if (isA)
		A = val;
	else
		StoreValAtCurrentAddr(val);
}

void CPU6502::ROR() {
	// Rotate Right
	// Move each of the bits in either A or M one place to the right.
	// Bit 7 is filled with the current value of the carry flag whilst the old bit 0 becomes the new carry flag value.
	bool isA = mCurrentInstruction.AddrFunc == &CPU6502::Accumulator;
	u8 val = isA ? A : GetValAtCurrentAddr();
	u8 tmp = val;
	val >>= 1;
	SetBitTo(val, 7, PS.C);
	PS.C = CheckBit(tmp, 0);
	HandleNZFlags(val);
	if (isA)
		A = val;
	else
		StoreValAtCurrentAddr(val);
}

void CPU6502::JMP() {
	// Jump
	// Sets the program counter to the address specified by the operand.
	PC = mCurrentAddr;
	mIsJump = true;
}

void CPU6502::JSR() {
	// The JSR instruction pushes the address (minus one) of the return point
	// on to the stack and then sets the program counter to the target memory address.
	PushAddressToStack(PC + mCurrentInstruction.length - 1);
	PC = mCurrentAddr;
	mIsJump = true;
}

void CPU6502::RTS() {
	// Return from Subroutine
	// The RTS instruction is used at the end of a subroutine to return to the calling routine.
	// It pulls the program counter (minus one) from the stack.
	// TODO: Minus One?
	PC = PullAddressFromStack() + 1;
	mIsJump = true;
}

void CPU6502::BCC() {
	// Branch if Carry Clear
	// If the carry flag is clear then add the relative displacement to the program counter to cause a branch to a new location.
	// TODO: Cycles +1 if branch succeeds +2 if to a new page
	Branch(!PS.C);
}

void CPU6502::BCS() {
	// Branch if Carry Set
	// TODO: Cycles +1 if branch succeeds +2 if to a new page
	Branch(PS.C);
}

void CPU6502::BEQ() {
	// Branch if Equal
	// If the zero flag is set then add the relative displacement to the program counter to cause a branch to a new location.
	// TODO: Cycles +1 if branch succeeds +2 if to a new page
	Branch(PS.Z);
}

void CPU6502::BMI() {
	// Branch if Minus
	// If the negative flag is set then add the relative displacement to the program counter to cause a branch to a new location.
	// TODO: Cycles +1 if branch succeeds +2 if to a new page
	Branch(PS.N);
}

void CPU6502::BNE() {
	// Branch if Not Equal
	// If the zero flag is clear then add the relative displacement to the program counter to cause a branch to a new location.
	// TODO: Cycles +1 if branch succeeds +2 if to a new page
	Branch(!PS.Z);
}

void CPU6502::BPL() {
	// Branch if Positive
	// If the negative flag is clear then add the relative displacement to the program counter to cause a branch to a new location.
	// TODO: Cycles +1 if branch succeeds +2 if to a new page
	Branch(!PS.N);
}

void CPU6502::BVC() {
	// Branch if Overflow Clear
	// If the overflow flag is clear then add the relative displacement to the program counter to cause a branch to a new location.
	// TODO: Cycles +1 if branch succeeds +2 if to a new page
	Branch(!PS.V);
}

void CPU6502::BVS() {
	// Branch if Overflow Set
	// If the overflow flag is set then add the relative displacement to the program counter to cause a branch to a new location.
	// TODO: Cycles +1 if branch succeeds +2 if to a new page
	Branch(PS.V);
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
	// Set Carry Flag
	// C = 1
	PS.C = 1;
}

void CPU6502::SED() {
	// Set Decimal Flag
	// D = 1
	PS.D = 1;
}

void CPU6502::SEI() {
	// Set Interrupt Disable
	// I = 1
	PS.I = 1;
}

void CPU6502::BRK() {
	// Force Interrupt
	// The BRK instruction forces the generation of an interrupt request.
	// The program counter and processor status are pushed on the stack then the IRQ interrupt vector at $FFFE/F
	// is loaded into the PC and the break flag in the status set to one.
	PushAddressToStack(PC);
	PS.B = 1;
	PushToStack(PS.data);
	PS.B = 0;
	PC = mC64->load16(0xFFFE);
}

void CPU6502::NOP() {
}

void CPU6502::RTI() {
	// Return from Interrupt
	// The RTI instruction is used at the end of an interrupt processing routine. It pulls the processor flags from the stack followed by the program counter.
	PS.data = PullFromStack();
	PC = PullAddressFromStack();
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
