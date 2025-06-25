#pragma once
#include "Utility\Includes.hpp"
#include "C64\Component.hpp"

template <u32 size>
class Memory : public Component {
public:
	Memory(const std::string &name) : Component(name) {
		Reset();
	};

	auto IOLoad8(u16 address) const -> u8 override {
		return *((u8 *)(&mData[address]));
	}

	auto IOLoad16(u16 address) const -> u16 override {
		return *((u16 *)(&mData[address]));
	}

	auto IOStore8(u16 address, u8 value) -> void override {
		mData[address] = value;
	}
	auto IOStore16(u16 address, u16 value) -> void override {
		mData[address + 0] = (u8)value;
		mData[address + 1] = (u8)(value >> 8);
	}

	auto loadFromFile(std::string filePath) -> bool {
		Reset();
		if (FILE *file = fopen(filePath.c_str(), "rb+")) {
			fread(mData, sizeof(u8), size, file);
			fclose(file);
			log->info("{} erfolgreich geladen", filePath);
			return true;
		}
		log->error("{} nicht vorhanden", filePath);
		return false;
	}

	auto Reset() -> void {
		memset(mData, 0, size_t(size));
	}

	auto Data() -> u8 * { return mData; }

	auto Get() -> Memory<size> * {
		return this;
	}

private:
	u8 *mData = new u8[size];
	LogPtr log = AddLogger("MEM");
};