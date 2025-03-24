#pragma once

#include "Types.hpp"

template <typename T, typename = typename std::enable_if<std::is_integral<T>::value>::type>
constexpr bool CheckBit(T num, i32 bit) {
	return (num & (1 << bit)) != 0;
}

template <typename T, typename = typename std::enable_if<std::is_integral<T>::value>::type>
constexpr void ToggleBit(T &num, i32 bit) {
	num ^= (1 << bit);
}

template <typename T, typename = typename std::enable_if<std::is_integral<T>::value>::type>
constexpr void ClearBit(T &num, i32 bit) {
	num &= ~(1 << bit);
}

template <typename T, typename = typename std::enable_if<std::is_integral<T>::value>::type>
constexpr void ClearBitIf(T &num, i32 bit, bool check) {
	num &= ~(1 << bit);
}

template <typename T, typename = typename std::enable_if<std::is_integral<T>::value>::type>
constexpr void SetBit(T &num, i32 bit) {
	num |= (1 << bit);
}

template <typename T, typename = typename std::enable_if<std::is_integral<T>::value>::type>
constexpr void SetBitIf(T &num, i32 bit, bool check) {
	if (check)
		num |= (1 << bit);
}

template <typename T, typename = typename std::enable_if<std::is_integral<T>::value>::type>
constexpr void SetBitTo(T &num, i32 bit, bool check) {
	// Bit == check
	if (check) {
		num |= (1 << bit);
	} else {
		num &= ~(1 << bit);
	}
}