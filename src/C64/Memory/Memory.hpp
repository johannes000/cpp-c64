#pragma once
#include "Utility\Includes.hpp"
#include "C64\Component.hpp"

template <u32 size>
class Memory : public Component {
public:
	Memory(const std::string &name) : Component(name) {
		log = spdlog::get("MEM");

		Reset();
	};

	u8 IOLoad8(u16 address) const override {
		return *((u8 *)(&mData[address]));
	}
	u16 IOLoad16(u16 address) const override {
		return *((u16 *)(&mData[address]));
	}

	void IOStore8(u16 address, u8 value) override {
		mData[address] = value;
	}
	void IOStore16(u16 address, u16 value) override {
		mData[address + 0] = (u8)value;
		mData[address + 1] = (u8)(value >> 8);
	}

	bool loadFromFile(std::string filePath) {
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

	void Reset() {
		memset(mData, 0, size_t(size));
	}

private:
	u8 *mData = new u8[size];
	std::shared_ptr<spdlog::logger> log;
};