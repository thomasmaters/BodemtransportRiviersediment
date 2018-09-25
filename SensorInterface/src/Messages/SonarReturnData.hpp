/*
 * SonarReturnData.hpp
 *
 *  Created on: 11 Jun 2018
 *      Author: Thomas Maters
 *		Email : thomasmaters@hotmail.com (TG.Maters@student.han.nl)
 */

#ifndef SRC_SONARRETURNDATA_HPP_
#define SRC_SONARRETURNDATA_HPP_

#include "../DataBuffer.hpp"
#include "SensorMessage.hpp"
#include "SonarReturnDataPacket.hpp"

//Values
#define IUX_PACKET_SIZE 8192
#define IUX_DATA_SIZE 8013
#define IVX_PACKET_SIZE 16384
#define IVX_DATA_SIZE 16013
#define SRD_MAX_PACKETS 16
#define SRD_DATA_BYTES_IUX 10
#define SRD_DATA_BYTES_IVX 11
#define SRD_TERMINATION_BYTE 0xFC
#define SRD_OP_FREQUENTY 675
#define SRD_PACKET_HEADER_LENGTH 12
#define SRD_DISPLAY_GAIN 87
#define SRD_REP_RATE 76

//Offsets
#define SRD_837_HEADER_OFFSET 0
#define SRD_DATA_BYTES_OFFSET 3
#define SRD_TOTAL_BYTES_HIGH 4
#define SRD_TOTAL_BYTES_LOW 5
#define SRD_TO_READ_BYTES_HIGH 6
#define SRD_TO_READ_BYTES_LOW 7
#define SRD_START_GAIN_OFFSET 38
#define SRD_SHIP_LATITUDE_OFFSET 48
#define SRD_SHIP_LONGITUDE_OFFSET 62
#define SRD_OP_FREQUENTY_HIGH 80
#define SRD_OP_FREQUENTY_LOW 81
#define SRD_REP_RATE_HIGH 88
#define SRD_REP_RATE_LOW 89
#define SRD_DISPLAY_GAIN_OFFSET 90
#define SRD_PACKET_HEADER_OFFSET 100
#define SRD_PACKET_DATA_OFFSET 112
#define SRD_TER_BYTE_IUX_OFFSET 8112
#define SRD_TER_BYTE_IVX_OFFSET 16112

namespace Messages
{
/**
 * Class that represents a combination of 8 or 16 SonarDataPackets.
 * Also known as raw sonar data aka .837 format.
 */
class SonarReturnData : public SensorMessage
{
	static constexpr std::size_t command_length_ = SonarReturnDataPacket::command_length_ * SRD_MAX_PACKETS;

  public:
    SonarReturnData() : SensorMessage(command_length_)
    {
    }

    virtual ~SonarReturnData()
    {

    }

    explicit SonarReturnData(std::unique_ptr<DataBuffer<>>& buffer)
      : SensorMessage(SonarReturnDataPacket::command_length_ * SRD_MAX_PACKETS)
    {
        for (std::size_t i = 0; i < buffer->size(); ++i)
        {
            SensorMessage message = buffer->asMessage(i);
            SonarReturnDataPacket packet = reinterpret_cast<SonarReturnDataPacket&>(message);

            addPacket(packet);
        }
    }

    /**
     * Adds a packets data to this message buffer.
     * @param packet To Add.
     */
    void addPacket(const SonarReturnDataPacket& packet)
    {
        std::pair<uint8_t*, uint16_t> a = packet.getEchoData();
        uint8_t packet_number = packet.getPacketNumber();
        if (packet_number == static_cast<std::underlying_type<Mode>::type>(packet.getMode()) - 1)
        {
            std::memcpy(&data_[SRD_PACKET_HEADER_OFFSET], packet.getData(), SRD_PACKET_HEADER_LENGTH);
        }
        if (packet_number == 0)
        {
            initDataHeader(packet);
        }
        std::memcpy(&data_[SRD_PACKET_DATA_OFFSET + packet_number * packet.getNumberOfDataBytes()], a.first, a.second);
    }

  private:
    /**
     * Uses a packet to init the header of this message.
     * @param packet
     */
    void initDataHeader(const SonarReturnDataPacket& packet)
    {
        std::memcpy(&data_[SRD_837_HEADER_OFFSET], "837", 3);

        if (packet.getMode() == Messages::Mode::IUX)
        {
            data_[SRD_DATA_BYTES_OFFSET]    = SRD_DATA_BYTES_IUX;  // Mode
            data_[SRD_TOTAL_BYTES_HIGH]    = (IUX_PACKET_SIZE >> 8) & 0xFF;
            data_[SRD_TOTAL_BYTES_LOW]    = (IUX_PACKET_SIZE & 0xFF);
            data_[SRD_TO_READ_BYTES_HIGH]    = (IUX_DATA_SIZE >> 8) & 0xFF;
            data_[SRD_TO_READ_BYTES_LOW]    = (IUX_DATA_SIZE & 0xFF);
            data_[SRD_TER_BYTE_IUX_OFFSET] = SRD_TERMINATION_BYTE;
        }
        else
        {
            data_[SRD_DATA_BYTES_OFFSET]     = SRD_DATA_BYTES_IVX;  // Mode
            data_[SRD_TOTAL_BYTES_HIGH]     = (IVX_PACKET_SIZE >> 8) & 0xFF;
            data_[SRD_TOTAL_BYTES_LOW]     = (IVX_PACKET_SIZE & 0xFF);
            data_[SRD_TO_READ_BYTES_HIGH]     = (IVX_DATA_SIZE >> 8) & 0xFF;
            data_[SRD_TO_READ_BYTES_LOW]     = (IVX_DATA_SIZE & 0xFF);
            data_[SRD_TER_BYTE_IVX_OFFSET] = SRD_TERMINATION_BYTE;
        }

        data_[SRD_START_GAIN_OFFSET] = packet.getCurrentSensorGain();
        std::memcpy(&data_[SRD_SHIP_LATITUDE_OFFSET], " dd.mm.xxxxx N", 14);
        std::memcpy(&data_[SRD_SHIP_LONGITUDE_OFFSET], "ddd.mm.xxxxx E", 14);
        data_[SRD_OP_FREQUENTY_HIGH] = (SRD_OP_FREQUENTY >> 8) & 0xFF;  // Operating frequency
        data_[SRD_OP_FREQUENTY_LOW] = (SRD_OP_FREQUENTY & 0xFF);       // Operating frequency
        data_[SRD_REP_RATE_HIGH] = (SRD_REP_RATE >> 8) & 0xFF;                  // Repetition rate (This one is important for file playback)
        data_[SRD_REP_RATE_LOW] = (SRD_REP_RATE & 0xFF);                 // Repetition rate (This one is important for file playback)
        data_[SRD_DISPLAY_GAIN_OFFSET] = SRD_DISPLAY_GAIN;                 // Display gain
    }
};
} //Namespace Messages

#endif /* SRC_SONARRETURNDATA_HPP_ */
