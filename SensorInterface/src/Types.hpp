/*
 * Types.hpp
 *
 *  Created on: 6 Jun 2018
 *      Author: Thomas Maters
 *		Email : thomasmaters@hotmail.com (TG.Maters@student.han.nl)
 */

#ifndef TYPES_HPP_
#define TYPES_HPP_

#include <cstdint>

template<typename type, uint8_t size>
struct U1_t{};

//typedef U1_t<uint8_t,1> U1;

typedef std::uint8_t U1;
typedef std::uint16_t U2;
struct U3 {
  uint32_t value : 24;

  std::ostream& operator<<(std::ostream& os)
  {
	  os << value;
      return os;
  }

  std::istream& operator>>(std::istream& is)
  {
	  char* buffer = new char[3];
	  is.read(buffer, 3);
	  value = (buffer[0] << 16) | (buffer[1] << 8) | buffer[2];
	  delete buffer;
	  return is;
  }
};
typedef std::uint32_t U4;

typedef std::int8_t S1;
typedef std::int16_t S2;
class S3 {
  int32_t value : 24;

  std::ostream& operator<<(std::ostream& os)
  {
	  os << value;
      return os;
  }

  std::istream& operator>>(std::istream& is)
  {
	  char* buffer = new char[3];
	  is.read(buffer, 3);
	  value = (buffer[0] << 16) | (buffer[1] << 8) | buffer[2];
	  delete buffer;
	  return is;
  }
};
typedef std::int32_t S4;
typedef float F4;

//static_assert(sizeof(U1) == 1, "U1 is not 1 byte in length!");
//static_assert(sizeof(U2) == 2, "U2 is not 2 bytes in length!");
//static_assert(sizeof(U4) == 4, "U4 is not 4 bytes in length!");
//
//static_assert(sizeof(S1) == 1, "S1 is not 1 byte in length!");
//static_assert(sizeof(S2) == 2, "S2 is not 2 bytes in length!");
//static_assert(sizeof(S4) == 4, "S4 is not 4 bytes in length!");
//
//static_assert(sizeof(F4) == 4, "F4 is not 4 bytes in length!");

#endif /* TYPES_HPP_ */
