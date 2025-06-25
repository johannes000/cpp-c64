#pragma once

#include "Types.hpp"

template <typename T, typename = typename std::enable_if<std::is_integral<T>::value>::type>
constexpr auto CheckBit(T num, i32 bit) -> bool {
	return (num & (1 << bit)) != 0;
}

template <typename T, typename = typename std::enable_if<std::is_integral<T>::value>::type>
constexpr auto ToggleBit(T &num, i32 bit) -> void {
	num ^= (1 << bit);
}

template <typename T, typename = typename std::enable_if<std::is_integral<T>::value>::type>
constexpr auto ClearBit(T &num, i32 bit) -> void {
	num &= ~(1 << bit);
}

template <typename T, typename = typename std::enable_if<std::is_integral<T>::value>::type>
constexpr auto ClearBitIf(T &num, i32 bit, bool check) -> void {
	num &= ~(1 << bit);
}

template <typename T, typename = typename std::enable_if<std::is_integral<T>::value>::type>
constexpr auto SetBit(T &num, i32 bit) -> void {
	num |= (1 << bit);
}

template <typename T, typename = typename std::enable_if<std::is_integral<T>::value>::type>
constexpr auto SetBitIf(T &num, i32 bit, bool check) -> void {
	if (check)
		num |= (1 << bit);
}

template <typename T, typename = typename std::enable_if<std::is_integral<T>::value>::type>
constexpr auto SetBitTo(T &num, i32 bit, bool check) -> void {
	// Bit == check
	if (check) {
		num |= (1 << bit);
	} else {
		num &= ~(1 << bit);
	}
}