#pragma once
#include "Utility/Includes.hpp"

class Component {
public:
	Component(const std::string &name) : mName(name) {
										 };
	virtual auto IOLoad8(u16 address) const -> u8;
	virtual auto IOLoad16(u16 address) const -> u16;

	virtual auto IOStore8(u16 address, u8 value) -> void;
	virtual auto IOStore16(u16 address, u16 value) -> void;

	virtual auto Reset() -> void = 0;

private:
	std::string mName;
	LogPtr log = AddLogger("C64");
};