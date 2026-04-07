#ifndef BITSET_HPP
#define BITSET_HPP

#include <cstdint>

class BitSet {
   public:
	BitSet() : bitset(0) {}
	BitSet(uint8_t bits);
	void	addFlag(uint8_t flag);
	void	removeFlag(uint8_t flag);
	void	reset();
	uint8_t getFlags() const;
	bool	isSet(uint8_t flag) const;

   private:
	uint8_t bitset;
};

#endif
