#pragma once

#include <cstdint>

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

typedef float f32;
typedef double f64;

#define global static

constexpr u32 kib(u32 x) { return 1024 * x; }
constexpr u32 mib(u32 x) { return 1024 * kib(x); }
constexpr u32 gib(u32 x) { return 1024 * mib(x); }

constexpr u16 KERNAL_ADDR = 0xE000;
constexpr u16 KERNAL_SIZE = kib(8);
constexpr u16 BASIC_ROM_ADDR = 0xA000;
constexpr u16 BASIC_ROM_SIZE = kib(8);
constexpr u16 CHARACTER_ROM_ADDR = 0xD000;
constexpr u16 CHARACTER_ROM_SIZE = kib(4);
constexpr u16 RAM_ADDR = 0x0000;
constexpr u16 RAM_SIZE = kib(39);

namespace AddressRange {
struct AddressRange {
	constexpr AddressRange(u16 start, u16 size) : mStart(start), mEnd(static_cast<u16>(std::min<u32>(start + size, 0xFFFF))), mSize(size) {};
	bool Contains(u16 addr, u16 &relativeAddr) const {
		if (addr >= mStart && addr < mEnd) {
			relativeAddr = addr - mStart;
			return true;
		}
		return false;
	}
	u16 mStart;
	u16 mEnd;
	u16 mSize;
};

constexpr AddressRange BASIC_ROM(BASIC_ROM_ADDR, BASIC_ROM_SIZE);
constexpr AddressRange CHARACTER_ROM(CHARACTER_ROM_ADDR, CHARACTER_ROM_SIZE);
constexpr AddressRange KERNAL(KERNAL_ADDR, KERNAL_SIZE);
constexpr AddressRange RAM(RAM_ADDR, RAM_SIZE);
}; // namespace AddressRange