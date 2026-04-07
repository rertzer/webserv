#include "BitSet.hpp"
#include <iostream>

BitSet::BitSet(uint8_t bits) : bitset(bits) {}

void BitSet::addFlag(uint8_t flag) {
	bitset |= flag;
}

void BitSet::removeFlag(uint8_t flag) {
	bitset &= ~flag;
}

uint8_t BitSet::getFlags() const {
	return bitset;
}

bool BitSet::isSet(uint8_t flag) const {
	return (bitset & flag);
}

void BitSet::reset() {
	bitset = 0;
}
