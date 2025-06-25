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

	CPU6502 *GetCpu() { return mCPU.get(); };

	u8 *GetBasicRomData() { return mBasic.Data(); }
	u8 *GetCharSetData() { return mCharSet.Data(); }
	u8 *GetKernalData() { return mKernal.Data(); }
	u8 *GetRamData() { return mRam.Data(); }

	Memory<BASIC_ROM_SIZE> *GetBasicPointer() {
		return &mBasic;
	}
	Memory<CHARACTER_ROM_SIZE> *GetCharSetPointer() {
		return &mCharSet;
	}
	Memory<KERNAL_SIZE> *GetKernalPointer() {
		return &mKernal;
	}
	Memory<RAM_SIZE> *GetRamPointer() {
		return &mRam;
	}

	std::vector<std::string> mCpuLogs;

private:
	Memory<BASIC_ROM_SIZE> mBasic;
	Memory<CHARACTER_ROM_SIZE> mCharSet;
	Memory<KERNAL_SIZE> mKernal;
	Memory<RAM_SIZE> mRam;

	std::unique_ptr<CPU6502> mCPU;

private:
	LogPtr log = AddLogger("C64");
};