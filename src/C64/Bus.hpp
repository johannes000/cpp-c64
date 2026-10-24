#pragma once
#include "Utility/Types.hpp"

// Speicherzugriff der CPU. Der C64 bildet darüber seine Speicherkarte ab,
// Tests können ein flaches 64K-RAM anbinden.
class Bus {
public:
	virtual ~Bus() = default;

	virtual auto load8(u16 address) -> u8 = 0;
	virtual auto store8(u16 address, u8 value) -> void = 0;
};
