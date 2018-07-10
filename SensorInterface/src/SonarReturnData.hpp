/*
 * SonarReturnData.hpp
 *
 *  Created on: 11 Jun 2018
 *      Author: Thomas Maters
 *		Email : thomasmaters@hotmail.com (TG.Maters@student.han.nl)
 */

#ifndef SRC_SONARRETURNDATA_HPP_
#define SRC_SONARRETURNDATA_HPP_

#include "SensorMessage.hpp"
#include "SonarReturnDataPacket.hpp"
#include "DataBuffer.hpp"

#include <string>

namespace Controller::DeltaT100
{
	//Also known as raw sonar data aka .837 format.
	class SonarReturnData : public SensorMessage
	{
public:
		SonarReturnData(): SensorMessage(SonarReturnDataPacket::command_length_* 16)
		{

		}

		SonarReturnData(std::unique_ptr<DataBuffer<>>& buffer): SensorMessage(SonarReturnDataPacket::command_length_* 16)
		{
		    for (std::size_t i = 0; i < buffer->size(); ++i)
		    {
		        SensorMessage message                               = buffer->asMessage(i);
		        Controller::DeltaT100::SonarReturnDataPacket sonar_return = (Controller::DeltaT100::SonarReturnDataPacket&)message;
		        std::pair<uint8_t*, uint16_t> a = sonar_return.getEchoData();
		        if (i == 7)
		        {
		            std::memcpy(&data_[100], sonar_return.getData(), 12);
		        }
		        std::memcpy(&data_[112 + i * 1000], a.first, a.second);
		    }
		}

		void addPacket(SonarReturnDataPacket& packet)
		{
			std::pair<uint8_t*, uint16_t> a = packet.getEchoData();
			uint8_t packet_number = packet.getPacketNumber();
	        if (packet_number == 7)
	        {
	            std::memcpy(&data_[100], packet.getData(), 12);
	        }
			std::memcpy(&data_[112 + packet_number * 1000], a.first, a.second);
		}
private:
	};

}



#endif /* SRC_SONARRETURNDATA_HPP_ */
