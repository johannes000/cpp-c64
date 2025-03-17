#pragma once
#include "Utility/Includes.hpp"
#include "CPU/CPU6502.hpp"

class C64 {
public:
	C64();
	~C64();

private:
	std::unique_ptr<CPU6502> mCPU;

private:
	std::shared_ptr<spdlog::logger> log;
};