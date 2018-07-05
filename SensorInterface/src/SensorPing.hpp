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

/**
 * Template so a function call in the form of toFormat<FileType,PingType>(ostream, PingType)
 */
template <typename PingType, typename FileType>
void toFormat(std::ostream& stream, PingType& ping, FileType)
{
}

/**
 * Confenieance template function so a user can call it in this form: toFormat<FileType>(ostream, pingInstance)
 */
template <typename FileType, typename PingType>
void toFormat(std::ostream& stream, PingType& ping)
{
    toFormat<PingType, FileType>(stream, ping, {});
}

class SensorPing
{
  public:
    SensorPing() : time_of_ping_(std::chrono::system_clock::now())
    {
    }

    SensorPing(const SensorPing& rhs) : time_of_ping_(rhs.time_of_ping_)
    {
    }

    SensorPing& operator=(const SensorPing& rhs)
    {
        if (this != &rhs)
        {
            time_of_ping_ = rhs.time_of_ping_;
        }
        return *this;
    }

    virtual void getTimeOfPing()
    {
    }

    virtual ~SensorPing()
    {
    }

  private:
    std::chrono::_V2::system_clock::time_point time_of_ping_;
};

#endif /* SRC_SENSORPING_HPP_ */
