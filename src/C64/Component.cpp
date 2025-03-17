#include "Component.hpp"

u8 Component::IoLoad8(u16 address) const {
	log->info("{} {} {:X}", mName, __FUNCTION__, address);
}
u16 Component::IoLoad16(u16 address) const {
	log->info("{} {} {:X}", mName, __FUNCTION__, address);
}

void Component::IoStore8(u16 address, u8 value) {
	log->info("{} {} {:X} {:X}", mName, __FUNCTION__, address, value);
}
void Component::IoStore16(u16 address, u16 value) {
	log->info("{} {} {:X} {:X}", mName, __FUNCTION__, address, value);
}