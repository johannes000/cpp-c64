#pragma once
#include "Utility/Includes.hpp"
#include "CPU/CPU6502.hpp"

class Component;

class C64 {
public:
	C64();
	~C64();

	void RunOneInstruction();
	void Reset();

	Component *Decode(u16 address, u16 &relativeAddress);

	u8 load8(u16 address);
	u16 load16(u16 address);
	void store8(u16 address, u8 value);
	void store16(u16 address, u16 value);

private:
	std::unique_ptr<CPU6502> mCPU;

	Memory<KERNAL_SIZE> mKernal;
	Memory<BASIC_ROM_SIZE> mBasic;
	Memory<CHARACTER_ROM_SIZE> mCharSet;

private:
	std::shared_ptr<spdlog::logger> log;
};