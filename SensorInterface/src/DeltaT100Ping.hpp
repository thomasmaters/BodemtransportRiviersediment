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
#include "SonarReturnDataPacket.hpp"
#include "SwitchDataCommand.hpp"
#include "ProfilePointOutput.hpp"

#include <boost/format.hpp>

#include <algorithm>
#include <chrono>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <iostream>

struct DeltaT837
{
};

struct Ascii
{
};

namespace Controller::DeltaT100
{
class DeltaT100Ping : public SensorPing
{
  public:
    template <std::size_t Size>
    DeltaT100Ping(std::unique_ptr<DataBuffer<Size>>& buffer) : buffer_(std::move(buffer))
    {
    }

    DeltaT100Ping(const DeltaT100Ping& rhs) = delete;
    DeltaT100Ping& operator=(const DeltaT100Ping& rhs) = delete;

    virtual ~DeltaT100Ping()
    {
    }

    std::unique_ptr<DataBuffer<>> buffer_;
};
}

template <>
void toFormat<Ascii, Controller::DeltaT100::DeltaT100Ping>(std::ostream& stream,
                                                           Controller::DeltaT100::DeltaT100Ping& ping)
{
}

template <>
void toFormat<DeltaT837, Controller::DeltaT100::DeltaT100Ping>(std::ostream& o,
                                                               Controller::DeltaT100::DeltaT100Ping& ping)
{
    uint8_t* data;
    std::size_t length;

    SensorMessage message                             = ping.buffer_->asMessage(0);
    Controller::DeltaT100::SonarReturnDataPacket sonar_data = (Controller::DeltaT100::SonarReturnDataPacket&)message;

    if (sonar_data.getMode() == Controller::DeltaT100::Mode::IUX)
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
    ss3 << boost::format(".%|03|") % std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(
                                                        now - std::chrono::time_point_cast<std::chrono::seconds>(now))
                                                        .count());
    std::string milis = ss3.str().substr(0, 3) + '\0';

    std::memcpy(&data[8], date.data(), 12);
    std::memcpy(&data[20], time.data(), 9);
    std::memcpy(&data[29], milis.data(), 4);
    data[38] = sonar_data.getCurrentSensorGain();
    std::memcpy(&data[48], " dd.mm.xxxxx N", 14);
    std::memcpy(&data[62], "ddd.mm.xxxxx E", 14);
    data[80] = (675 >> 8) & 0xFF;
    data[81] = (675 & 0xFF);
    data[88] = 0;
    data[89] = 76;
    data[90] = 86;

    for (std::size_t i = 0; i < ping.buffer_->size(); ++i)
    {
        SensorMessage message                               = ping.buffer_->asMessage(i);
        Controller::DeltaT100::SonarReturnDataPacket sonar_return = (Controller::DeltaT100::SonarReturnDataPacket&)message;
        std::pair<uint8_t*, uint16_t> a = sonar_return.getEchoData();
        if (i == 7)
        {
            std::memcpy(&data[100 + i * 1000], sonar_return.getData(), 12);
        }
        std::memcpy(&data[112 + i * 1000], a.first, a.second);
    }

    o.write((char*)&data[0], length);
    delete data;
}
#endif /* SRC_DELTAT100PING_HPP_ */
