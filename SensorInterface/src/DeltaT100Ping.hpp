/*
 * DeltaT100Ping.hpp
 *
 *  Created on: 2 Jul 2018
 *      Author: Thomas Maters
 *		Email : thomasmaters@hotmail.com (TG.Maters@student.han.nl)
 */

#ifndef SRC_DELTAT100PING_HPP_
#define SRC_DELTAT100PING_HPP_

#include "SensorPing.hpp"
#include "SonarReturnData.hpp"
#include "SwitchDataCommand.hpp"

#include <boost/format.hpp>

#include <chrono>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <iostream>

class DeltaT837
{
  public:
    DeltaT837()                               = default;
    virtual void toDeltaT837(std::ostream& o) = 0;
    virtual ~DeltaT837()
    {
    }
};

namespace Controller::DeltaT100
{
class DeltaT100Ping : public SensorPing, public DeltaT837
{
  public:
    template <std::size_t Size>
    DeltaT100Ping(std::unique_ptr<DataBuffer<Size>>& buffer) : buffer_(std::move(buffer))
    {
    }

    DeltaT100Ping(const DeltaT100Ping& rhs) = delete;
    DeltaT100Ping& operator=(const DeltaT100Ping& rhs) = delete;

    void toDeltaT837(std::ostream& o)
    {
        uint8_t* data;
        std::size_t length;

        SensorMessage message      = buffer_->asMessage(0);
        SonarReturnData sonar_data = (SonarReturnData&)message;

        if (sonar_data.getMode() == Mode::IUX)
        {
            length     = 8192;
            data       = new uint8_t[8192]{ 0 };
            data[3]    = 10;
            data[4]    = (length >> 8) & 0xFF;
            data[5]    = (length & 0xFF);
            data[6]    = (8013 >> 8) & 0xFF;
            data[7]    = (8013 & 0xFF);
            data[8112] = 0xFC;
        }
        else
        {
            length      = 16384;
            data        = new uint8_t[16384]{ 0 };
            data[3]     = 11;
            data[4]     = (length >> 8) & 0xFF;
            data[5]     = (length & 0xFF);
            data[6]     = (16013 >> 8) & 0xFF;
            data[7]     = (16013 & 0xFF);
            data[16112] = 0xFC;
        }

        std::memcpy(&data[0], "837", 3);
        auto now       = std::chrono::system_clock::now();
        auto in_time_t = std::chrono::system_clock::to_time_t(now);

        std::stringstream ss;
        std::stringstream ss2;
        std::stringstream ss3;
        ss << std::put_time(std::localtime(&in_time_t), "%d-%b-%Y\0");
        std::string date = ss.str();
        ss2 << std::put_time(std::localtime(&in_time_t), "%H:%M:%S\0");
        std::string time = ss2.str();
        ss3 << boost::format(".%|03|") %
                   std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(
                                      now - std::chrono::time_point_cast<std::chrono::seconds>(now))
                                      .count());
        std::string milis = ss3.str().substr(0, 3) + '\0';

        std::memcpy(&data[8], date.data(), 12);
        std::memcpy(&data[20], time.data(), 9);
        std::memcpy(&data[29], milis.data(), 4);
        std::memcpy(&data[48], " dd.mm.xxxxx N", 14);
        std::memcpy(&data[62], "ddd.mm.xxxxx E", 14);
        data[88] = 0;
        data[89] = 76;

        for (std::size_t i = 0; i < buffer_->size(); ++i)
        {
            SensorMessage message        = buffer_->asMessage(i);
            SonarReturnData sonar_return = (SonarReturnData&)message;
            uint8_t* b                   = sonar_return.getData();
            std::pair<uint8_t*, uint16_t> a = sonar_return.getEchoData();
            std::memcpy(&data[100 + i * 1000], b, 12);
            std::memcpy(&data[112 + i * 1000], a.first, a.second);
        }

        o.write((char*)&data[0], length);
        delete data;
    }

    virtual ~DeltaT100Ping()
    {
    }

  private:
    std::unique_ptr<DataBuffer<>> buffer_;
};
}

#endif /* SRC_DELTAT100PING_HPP_ */
