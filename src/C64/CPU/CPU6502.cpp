#include "CPU6502.hpp"

namespace {
constexpr std::array<const char *, 3> Kernals = {
	"roms/kernal.901227-01.bin",
	"roms/kernal.901227-02.bin",
	"roms/kernal.901227-03.bin"};

constexpr std::array<const char *, 1> CharSets = {
	"roms/characters.901225-01.bin"};

constexpr std::array<const char *, 1> BasicRoms = {
	"roms/basic.901226-01.bin"};
} // namespace

CPU6502::CPU6502() : Component("CPU"),
					 mKernal("Kernal"),
					 mBasic("Basic"),
					 mCharSet("Char"),
					 A(0), X(0), Y(0),
					 PS(0),
					 SP(0),
					 PC(0),
					 log(spdlog::get("CPU")) {
	assert(OpCodes.size() == 256);
	mKernal.loadFromFile(Kernals[2]);
	mBasic.loadFromFile(CharSets[0]);
	mCharSet.loadFromFile(BasicRoms[0]);

	log->info("{:X}", mKernal.IoLoad8(0xFF));
	log->info("{:X}", mKernal.IoLoad16(0xFF));

	(this->*(OpCodes[0x00].InstFunc))();
	(this->*(OpCodes[0x01].InstFunc))();
	(this->*(OpCodes[0x02].InstFunc))();

	// auto f = (this->*OpCodes[3].InstFunc)();
}

CPU6502::~CPU6502() {
}
