#pragma once

#include <Utility\Includes.hpp>
#include "imgui.h"

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
public:
	Debugger(C64 *c64);

	void RenderDebugger();
	void Update();

private:
	void ShowC64CpuControls();
	void ShowC64CpuStats();
	void ShowDecompiler();
	void ShowMemoryRange(const char *label, const u8 *data, u16 startAddress, u16 size);
	void ShowMemoryView();

	void DecompileKernal();

private:
	C64 *mC64;
	std::vector<DebuggerDeconInst> mKernalDecon;
	std::shared_ptr<spdlog::logger> log;
};