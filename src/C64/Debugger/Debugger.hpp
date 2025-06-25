#pragma once

#include <Utility\Includes.hpp>
#include "imgui.h"

class CPU6502;
class C64;

struct DebuggerDeconInst {
	u16 addr{0};
	u8 opCode{0};
	u8 length{0};
	u8 cycles{0};
	u16 parameter{0};
	std::string relAddr;
	std::string mnemonic;
};

class Debugger {
	friend class CPU6502;
	friend class C64;

public:
	Debugger(C64 *c64);

	auto RenderDebugger() -> void;
	auto Update() -> void;

private:
	auto ShowC64CpuControls() -> void;
	auto ShowC64CpuStats() -> void;
	auto ShowDecompiler() -> void;
	auto ShowMemoryRange(const char *label, const u8 *data, u16 startAddress, u16 size) -> void;
	auto ShowMemoryView() -> void;

	auto DecompileKernal() -> void;

private:
	C64 *mC64;
	std::vector<DebuggerDeconInst> mKernalDecon;
	LogPtr log = AddLogger("DEB");
};