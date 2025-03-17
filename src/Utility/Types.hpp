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
