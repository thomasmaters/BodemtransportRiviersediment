/*
 * SensorPing.hpp
 *
 *  Created on: 28 Jun 2018
 *      Author: Thomas Maters
 *		Email : thomasmaters@hotmail.com (TG.Maters@student.han.nl)
 */

#ifndef SRC_SENSORPING_HPP_
#define SRC_SENSORPING_HPP_

#include "DataBuffer.hpp"

#include <chrono>
#include <cstdint>

template <class B, class = void>
struct is_good : std::false_type
{
};

template <class B>
struct is_good<B, std::void_t<decltype(std::declval<B&>() = std::declval<B>().toTest(std::declval<uint8_t*>()))>>
    : std::true_type
{
};

class SensorPing
{
  public:
    SensorPing()
    {
    }

    virtual void getFileHeader()
    {
    }

    virtual void getFileDataPortion()
    {
    }

    //	template<typename Astruct>
    //	Astruct toStruct(std::size_t size, std::size_t number)
    //	{
    //		static_assert(is_good<Astruct>::value,"Struct is no good");
    //		Astruct kaas;
    //		return kaas.toTest(&data_[0]);
    //	}

    virtual ~SensorPing()
    {
    }
};

struct FouteTest
{
};

struct Test
{
    float x, y, z;
    uint32_t empty;

    Test& toTest(uint8_t* data)
    {
        x     = (data[0] << 24) | (data[1] << 16) | (data[2] << 8) | data[3];
        y     = (data[4] << 24) | (data[5] << 16) | (data[6] << 8) | data[7];
        z     = (data[8] << 24) | (data[9] << 16) | (data[10] << 8) | data[11];
        empty = 0;
        return *this;
    }
};

#endif /* SRC_SENSORPING_HPP_ */
