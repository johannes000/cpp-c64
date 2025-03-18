#include "Component.hpp"

u8 Component::IOLoad8(u16 address) const {
	log->info("{} {} {:X}", mName, __FUNCTION__, address);
	return 0;
}
u16 Component::IOLoad16(u16 address) const {
	log->info("{} {} {:X}", mName, __FUNCTION__, address);
	return 0;
}

void Component::IOStore8(u16 address, u8 value) {
	log->info("{} {} {:X} {:X}", mName, __FUNCTION__, address, value);
}
void Component::IOStore16(u16 address, u16 value) {
	log->info("{} {} {:X} {:X}", mName, __FUNCTION__, address, value);
}