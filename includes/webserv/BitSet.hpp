#ifndef INCLUDES_WEBSERV_BITSET_HPP_
#define INCLUDES_WEBSERV_BITSET_HPP_

#include <cstdint>

class BitSet {
 public:
  BitSet() : bitset(0) {}
  explicit BitSet(uint8_t bits);
  void    addFlag(uint8_t flag);
  void    removeFlag(uint8_t flag);
  void    reset();
  uint8_t getFlags() const;
  bool    isSet(uint8_t flag) const;

 private:
  uint8_t bitset;
};

#endif  // INCLUDES_WEBSERV_BITSET_HPP_
