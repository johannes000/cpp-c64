#include "C64.hpp"

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

C64::C64() : mCPU(std::make_unique<CPU6502>(this)),
			 mKernal("Kernal"),
			 mBasic("Basic"),
			 mCharSet("Char"),
			 log(spdlog::get("C64")) {
	Reset();
}

C64::~C64() {
}

void C64::RunOneInstruction() {
	for (i32 i = 0; i < 10; i++) {
		mCPU->RunOneInstruction();
	}
}

void C64::Reset() {
	mKernal.loadFromFile(Kernals[2]);
	mBasic.loadFromFile(CharSets[0]);
	mCharSet.loadFromFile(BasicRoms[0]);
	mCPU->Reset();
}

Component *C64::Decode(u16 address, u16 &relativeAddress) {
	if (AddressRange::BASIC_ROM.Contains(address, relativeAddress))
		return &mBasic;
	if (AddressRange::KERNAL.Contains(address, relativeAddress))
		return &mKernal;
	if (AddressRange::CHARACTER_ROM.Contains(address, relativeAddress))
		return &mCharSet;

	return nullptr;
}

u8 C64::load8(u16 address) {
	u16 relAddress;

	auto component = Decode(address, relAddress);
	if (component == nullptr)
		return 0;
	return component->IOLoad8(relAddress);
}

u16 C64::load16(u16 address) {
	if (address % 2 != 0)
		log->critical("Datenzugriff nicht richtig ausgerichtet. Funk: {} Addr: {:X}", __FUNCTION__, address);
	u16 relAddress;

	auto component = Decode(address, relAddress);
	if (component == nullptr)
		return 0;
	return component->IOLoad16(relAddress);
}

void C64::store8(u16 address, u8 value) {
	u16 relAddress;

	auto component = Decode(address, relAddress);
	if (component != nullptr)
		return component->IOStore8(relAddress, value);
}

void C64::store16(u16 address, u16 value) {
	if (address % 2 != 0)
		log->critical("Datenzugriff nicht richtig ausgerichtet. Funk: {} Addr: {:X}", __FUNCTION__, address);
	u16 relAddress;

	auto component = Decode(address, relAddress);
	if (component != nullptr)
		return component->IOStore16(relAddress, value);
}