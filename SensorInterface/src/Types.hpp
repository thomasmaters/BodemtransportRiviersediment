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

typedef std::uint8_t U1;
typedef std::uint16_t U2;
typedef std::uint32_t U3;
typedef std::uint32_t U4;

typedef std::int8_t S1;
typedef std::int16_t S2;
typedef std::int32_t S3;
typedef std::int32_t S4;
typedef float F4;

struct U3_t {
  uint32_t value : 24;
};

static_assert(sizeof(U1) == 1, "U1 is not 1 byte!");
static_assert(sizeof(U2) == 2, "U2 is not 2 byte!");
static_assert(sizeof(U3_t) == 3, "U3 is not 3 byte!");
static_assert(sizeof(U4) == 4, "U4 is not 4 byte!");

static_assert(sizeof(S1) == 1, "S1 is not 1 byte!");
static_assert(sizeof(S2) == 2, "S2 is not 2 byte!");
static_assert(sizeof(S3) == 1, "S3 is not 3 byte!");
static_assert(sizeof(S4) == 4, "S4 is not 4 byte!");


#endif /* TYPES_HPP_ */
