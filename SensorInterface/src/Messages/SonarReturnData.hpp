/*
 * SonarReturnData.hpp
 *
 *  Created on: 11 Jun 2018
 *      Author: Thomas Maters
 *		Email : thomasmaters@hotmail.com (TG.Maters@student.han.nl)
 */

#ifndef SRC_SONARRETURNDATA_HPP_
#define SRC_SONARRETURNDATA_HPP_

#include "DataBuffer.hpp"
#include "SensorMessage.hpp"
#include "SonarReturnDataPacket.hpp"

#include <string>

#define IUX_PACKET_SIZE 8192
#define IUX_DATA_SIZE 8013
#define IVX_PACKET_SIZE 16384
#define IVX_DATA_SIZE 16013

namespace Controller::DeltaT100
{
// Also known as raw sonar data aka .837 format.
class SonarReturnData : public SensorMessage
{
  public:
    SonarReturnData() : SensorMessage(SonarReturnDataPacket::command_length_ * 16)
    {
    }

    explicit SonarReturnData(std::unique_ptr<DataBuffer<>>& buffer)
      : SensorMessage(SonarReturnDataPacket::command_length_ * 16)
    {
        for (std::size_t i = 0; i < buffer->size(); ++i)
        {
            SensorMessage message = buffer->asMessage(i);
            Controller::DeltaT100::SonarReturnDataPacket packet =
                (Controller::DeltaT100::SonarReturnDataPacket&)message;

            addPacket(packet);
        }
    }

    void addPacket(SonarReturnDataPacket& packet)
    {
        std::pair<uint8_t*, uint16_t> a = packet.getEchoData();
        uint8_t packet_number = packet.getPacketNumber();
        if (packet_number == static_cast<std::underlying_type<Mode>::type>(packet.getMode()) - 1)
        {
            std::memcpy(&data_[100], packet.getData(), 12);
        }
        if (packet_number == 0)
        {
            initDataHeader(packet);
        }
        std::memcpy(&data_[112 + packet_number * 1000], a.first, a.second);
    }

  private:
    void initDataHeader(SonarReturnDataPacket& packet)
    {
        std::memcpy(&data_[0], "837", 3);

        if (packet.getMode() == Controller::DeltaT100::Mode::IUX)
        {
            data_[3]    = 10;  // Mode
            data_[4]    = (IUX_PACKET_SIZE >> 8) & 0xFF;
            data_[5]    = (IUX_PACKET_SIZE & 0xFF);
            data_[6]    = (IUX_DATA_SIZE >> 8) & 0xFF;
            data_[7]    = (IUX_DATA_SIZE & 0xFF);
            data_[8112] = 0xFC;
        }
        else
        {
            data_[3]     = 11;  // Mode
            data_[4]     = (IVX_PACKET_SIZE >> 8) & 0xFF;
            data_[5]     = (IVX_PACKET_SIZE & 0xFF);
            data_[6]     = (IVX_DATA_SIZE >> 8) & 0xFF;
            data_[7]     = (IVX_DATA_SIZE & 0xFF);
            data_[16112] = 0xFC;
        }

        data_[38] = packet.getCurrentSensorGain();
        std::memcpy(&data_[48], " dd.mm.xxxxx N", 14);
        std::memcpy(&data_[62], "ddd.mm.xxxxx E", 14);
        data_[80] = (675 >> 8) & 0xFF;  // Operating frequency
        data_[81] = (675 & 0xFF);       // Operating frequency
        data_[88] = 0;                  // Repetition rate (This one is important for file playback)
        data_[89] = 76;                 // Repetition rate (This one is important for file playback)
        data_[90] = 86;                 // Display gain
    }
};
}

#endif /* SRC_SONARRETURNDATA_HPP_ */
