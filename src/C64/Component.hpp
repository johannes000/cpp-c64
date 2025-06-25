#pragma once
#include "Utility/Includes.hpp"

class Component {
public:
	Component(const std::string &name) : mName(name) {
										 };
	virtual u8 IOLoad8(u16 address) const;
	virtual u16 IOLoad16(u16 address) const;

	virtual void IOStore8(u16 address, u8 value);
	virtual void IOStore16(u16 address, u16 value);

	virtual void Reset() = 0;

private:
	std::string mName;
	LogPtr log = AddLogger("C64");
};