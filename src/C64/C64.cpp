#include "C64.hpp"

C64::C64() : mCPU(std::make_unique<CPU6502>()),
			 log(spdlog::get("C64")) {
}

C64::~C64() {
}