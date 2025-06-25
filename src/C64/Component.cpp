#include "Component.hpp"

auto Component::IOLoad8(u16 address) const -> u8 {
	log->info("{} {} {:X}", mName, __FUNCTION__, address);
	return 0;
}

auto Component::IOLoad16(u16 address) const -> u16 {
	log->info("{} {} {:X}", mName, __FUNCTION__, address);
	return 0;
}

auto Component::IOStore8(u16 address, u8 value) -> void {
	log->info("{} {} {:X} {:X}", mName, __FUNCTION__, address, value);
}
auto Component::IOStore16(u16 address, u16 value) -> void {
	log->info("{} {} {:X} {:X}", mName, __FUNCTION__, address, value);
}