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

#include <fstream>
#include <iomanip>
#include <iostream>
#include <chrono>
#include <ctime>

namespace Controller::DeltaT100
{
class DeltaT100Ping : public SensorPing
{
  public:
    DeltaT100Ping(const DataBuffer<>& buffer, Mode mode )
    {
    	SensorMessage message = buffer.asMessage(0);
    	SonarReturnData sonar_data = (SonarReturnData&)message;

    	if(sonar_data.getMode() == Mode::IUX)
    	{
    		data_ = new uint8_t[8192]{0};
    		data_[3] = 10;
    		data_[4] = (8192 & 0xFF);
    		data_[5] = (8192 >> 8) & 0xFF;
    		data_[6] = (8013 & 0xFF);
    		data_[7] = (8013 >> 8) & 0xFF;
        	data_[8112] = 0xFC;
    	}
    	else
    	{
    		data_ = new uint8_t[16384]{0};
    		data_[3] = 11;
    		data_[4] = (16384 & 0xFF);
    		data_[5] = (16384 >> 8) & 0xFF;
    		data_[6] = (16013 & 0xFF);
    		data_[7] = (16013 >> 8) & 0xFF;
    		data_[16111] = 0xFC;
    	}

    	data_[0] = '8';
    	data_[1] = '3';
    	data_[2] = '7';
        auto now = std::chrono::system_clock::now();
        auto in_time_t = std::chrono::system_clock::to_time_t(now);

        std::stringstream ss;
        std::stringstream ss2;
        ss << std::put_time(std::localtime(&in_time_t), "%d-%b-%Y\0");
        std::string date = ss.str();
        ss2 << std::put_time(std::localtime(&in_time_t), "%H:%M:%S\0");
        std::string time = ss2.str();
        std::cout << "Date: " << date << std::endl;
        std::cout << "Time: " << time << std::endl;
        auto seconds = std::chrono::time_point_cast<std::chrono::seconds>(now);
        auto fraction = now - seconds;
        auto milliseconds = "." + std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(fraction).count()/ 10).substr(0, 2) + '\0';

    	std::memcpy(&data_[8], date.data(), 12);
    	std::memcpy(&data_[20], time.data() , 9);
    	std::memcpy(&data_[29], milliseconds.data(), 4);

    	data_[38] = sonar_data.getGain();
    	data_[43] = 15;

    	std::memcpy(&data_[48], "_dd.mm.xxxxx_N", 14);
    	std::memcpy(&data_[62], "ddd.mm.xxxxx_E", 14);

    	for (std::size_t i = 0; i < buffer.size(); ++i)
    	{
    		SensorMessage message = buffer.asMessage(i);
        	SonarReturnData sonar_return = (SonarReturnData&)message;
        	uint8_t* b = sonar_return.getData();
        	std::pair<uint8_t*, uint16_t> a = sonar_return.getEchoData();
        	std::memcpy(&data_[100 + i * 1000], b, 12);
        	std::memcpy(&data_[112 + i * 1000], a.first, a.second);
		}

    	std::ofstream testDataOutputStream("kaas.837", std::ios::out | std::ios::app | std::ios::binary);
    	testDataOutputStream.write((char *)&data_[0], 8192);
    	testDataOutputStream.close();
    }

    void getFileHeader()
    {

    }

    void getFileDataPortion()
    {

    }

    virtual ~DeltaT100Ping()
    {
    }
  private:
    uint8_t* data_;
};
}

#endif /* SRC_DELTAT100PING_HPP_ */