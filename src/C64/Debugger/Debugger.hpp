#pragma once

#include <Utility\Includes.hpp>
#include "imgui.h"

class C64;

class Debugger {
public:
	Debugger(C64 *c64);

	void RenderDebugger();
	void ShowC64CpuControls();
	void ShowC64CpuStats();
	void ShowMemoryView();
	void ShowMemoryRange(const char *label, const u8 *data, u16 startAddress, u16 size);

private:
	C64 *mC64;
};