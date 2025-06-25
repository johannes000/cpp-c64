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

C64::C64() : mBasic("Basic"),
			 mCharSet("Char"),
			 mKernal("Kernal"),
			 mRam("Ram"),
			 mCPU(std::make_unique<CPU6502>(this)) {
	Reset();
}

C64::~C64() {
}

auto C64::RunOneInstruction() -> void {
	mCPU->RunOneInstruction();
	mCpuLogs.push_back(mCPU->GetCurrentInstructionLog());
}

auto C64::Reset() -> void {
	mKernal.loadFromFile(Kernals[2]);
	mBasic.loadFromFile(CharSets[0]);
	mCharSet.loadFromFile(BasicRoms[0]);

	mRam.Reset();
	mCPU->Reset();

	mCpuLogs.clear();
}

auto C64::Decode(u16 address, u16 &relativeAddress) -> Component * {
	if (AddressRange::BASIC_ROM.Contains(address, relativeAddress))
		return &mBasic;
	if (AddressRange::KERNAL.Contains(address, relativeAddress))
		return &mKernal;
	if (AddressRange::CHARACTER_ROM.Contains(address, relativeAddress))
		return &mCharSet;
	if (AddressRange::RAM.Contains(address, relativeAddress))
		return &mRam;
	log->warn("Keine Component unter {:04X}", address);
	return nullptr;
}

auto C64::load8(u16 address) -> u8 {
	u16 relAddress;

	auto component = Decode(address, relAddress);
	if (component == nullptr)
		return 0;
	return component->IOLoad8(relAddress);
}

auto C64::load16(u16 address) -> u16 {
	// if (address % 2 != 0)
	// 	log->critical("Datenzugriff nicht richtig ausgerichtet. Funk: {} Addr: {:X}", __FUNCTION__, address);
	u16 relAddress;

	auto component = Decode(address, relAddress);
	if (component == nullptr)
		return 0;
	return component->IOLoad16(relAddress);
}

auto C64::store8(u16 address, u8 value) -> void {
	u16 relAddress;

	auto component = Decode(address, relAddress);
	if (component != nullptr)
		return component->IOStore8(relAddress, value);
}

auto C64::store16(u16 address, u16 value) -> void {
	// if (address % 2 != 0)
	// 	log->critical("Datenzugriff nicht richtig ausgerichtet. Funk: {} Addr: {:X}", __FUNCTION__, address);
	u16 relAddress;

	auto component = Decode(address, relAddress);
	if (component != nullptr)
		return component->IOStore16(relAddress, value);
}