#pragma once
#include "Utility/Includes.hpp"

class Component {
public:
	Component(const std::string &name) : mName(name) {
		log = spdlog::get("C64");
	};
	virtual u8 IoLoad8(u16 address) const;
	virtual u16 IoLoad16(u16 address) const;

	virtual void IoStore8(u16 address, u8 value);
	virtual void IoStore16(u16 address, u16 value);

private:
	std::string mName;
	std::shared_ptr<spdlog::logger> log;
};