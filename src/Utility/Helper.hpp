#pragma once

#include "Types.hpp"

template <typename T, typename = typename std::enable_if<std::is_integral<T>::value>::type>
bool CheckBit(T num, int bit) {
	return (num & (1 << bit)) != 0;
}

template <typename T, typename = typename std::enable_if<std::is_integral<T>::value>::type>
void ToggleBit(T &num, int bit) {
	num ^= (1 << bit);
}

template <typename T, typename = typename std::enable_if<std::is_integral<T>::value>::type>
void ClearBit(T &num, int bit) {
	num &= ~(1 << bit);
}

template <typename T, typename = typename std::enable_if<std::is_integral<T>::value>::type>
void SetBit(T &num, int bit) {
	num |= (1 << bit);
}
