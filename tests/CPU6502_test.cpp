#include "C64/Bus.hpp"
#include "C64/CPU/CPU6502.hpp"

#include <gtest/gtest.h>

#include <array>
#include <cstdio>
#include <fstream>

namespace {

// Flaches 64K-RAM, wie es der Functional Test erwartet
class FlatRam : public Bus {
public:
	auto load8(u16 address) -> u8 override { return mData[address]; }
	auto store8(u16 address, u8 value) -> void override { mData[address] = value; }

	auto LoadFile(const char *path) -> bool {
		std::ifstream file(path, std::ios::binary);
		file.read(reinterpret_cast<char *>(mData.data()), mData.size());
		return file.gcount() == static_cast<std::streamsize>(mData.size());
	}

private:
	std::array<u8, 0x10000> mData{};
};

// Startadresse und Erfolgs-Trap laut bin_files/6502_functional_test.lst
constexpr u16 kStart = 0x0400;
constexpr u16 kSuccess = 0x3469;

} // namespace

TEST(CPU6502, KlausDormannFunctionalTest) {
	FlatRam ram;
	ASSERT_TRUE(ram.LoadFile(DORMANN_FUNCTIONAL_TEST_BIN)) << DORMANN_FUNCTIONAL_TEST_BIN;

	CPU6502 cpu(&ram);
	spdlog::set_level(spdlog::level::warn); // keine Ausgabe pro Befehl
	cpu.Reset();
	cpu.SetPC(kStart);

	// Jeder Fehler endet in einer Endlosschleife auf sich selbst (jmp * / bxx *),
	// der Erfolg ebenfalls, aber an kSuccess
	u16 pc = cpu.GetPC();
	u64 count = 0;
	for (; count < 200'000'000; ++count) {
		cpu.RunOneInstruction();
		if (cpu.GetPC() == pc)
			break;
		pc = cpu.GetPC();
	}

	std::printf("%llu Befehle ausgeführt, Ende bei $%04X\n", static_cast<unsigned long long>(count), pc);
	EXPECT_EQ(pc, kSuccess) << std::format("Trap bei ${:04X} nach {} Befehlen, siehe 6502_functional_test.lst", pc, count);
}
