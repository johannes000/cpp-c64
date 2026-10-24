#pragma once
#include "Utility/Includes.hpp"
#include "Bus.hpp"
#include "CPU/CPU6502.hpp"
#include "Memory/Memory.hpp"

class Component;

class C64 : public Bus {
public:
	C64();
	~C64();

	auto RunOneInstruction() -> void;
	auto Reset() -> void;

	auto Decode(u16 address, u16 &relativeAddress) -> Component *;

	auto load8(u16 address) -> u8 override;
	auto load16(u16 address) -> u16;
	auto store8(u16 address, u8 value) -> void override;
	auto store16(u16 address, u16 value) -> void;

	auto GetCpu() -> CPU6502 * { return mCPU.get(); };

	auto GetBasicRomData() -> u8 * { return mBasic.Data(); }
	auto GetCharSetData() -> u8 * { return mCharSet.Data(); }
	auto GetKernalData() -> u8 * { return mKernal.Data(); }
	auto GetRamData() -> u8 * { return mRam.Data(); }

	auto GetBasicPointer() -> Memory<BASIC_ROM_SIZE> * {
		return &mBasic;
	}
	auto GetCharSetPointer() -> Memory<CHARACTER_ROM_SIZE> * {
		return &mCharSet;
	}
	auto GetKernalPointer() -> Memory<KERNAL_SIZE> * {
		return &mKernal;
	}
	auto GetRamPointer() -> Memory<RAM_SIZE> * {
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