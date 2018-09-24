/*
 * SonarReturnDataPacket.hpp
 *
 *  Created on: 10 Jul 2018
 *      Author: Thomas Maters
 *		Email : thomasmaters@hotmail.com (TG.Maters@student.han.nl)
 */

#ifndef SRC_SONARRETURNDATAPACKET_HPP_
#define SRC_SONARRETURNDATAPACKET_HPP_

#include <cstdint>

#include "SensorMessage.hpp"

#define SRDP_CONVERT_FACTOR 360 / 65536
#define SRDP_PACKET_SIZE 1033

//Offsets
#define SRDP_MODE_OFFSET 1
#define SRDP_SERIAL_STATUS_OFFSET 4
#define SRDP_PACKET_NR_OFFSET 5
#define SRDP_FIRMWARE_V_OFFSET 6
#define SRDP_RANGE_OFFSET 7
#define SRDP_DATABYTES_HIGH 10
#define SRDP_DATABYTES_LOW 11
#define SRDP_EXTERNAL_TRIGGER_STATUS_OFFSET 12
#define SRDP_INTERAL_SENSOR_STATUS_OFFSET 13
#define SRDP_PITCH_HIGH 15
#define SRDP_PITCH_LOW 14
#define SRDP_ROLL_HIGH 17
#define SRDP_ROLL_LOW 16
#define SRDP_HEADING_HIGH 19
#define SRDP_HEADING_LOW 18
#define SRDP_TIMER_TICK_HIGH 21
#define SRDP_TIMER_TICK_LOW 20
#define SRDP_RUNMODE_OFFSET 22
#define SRDP_SENSOR_GAIN_OFFSET 24
#define SRDP_AGC_RANGE_HIGH 25
#define SRDP_AGC_RANGE_LOW 26
#define SRDP_AGC_MAX_HIGH 27
#define SRDP_AGC_MAX_LOW 28
#define SRDP_ECHO_DATA_OFFSET 32

namespace Messages
{
enum class SerialStatus : uint8_t
{
    SWITCHOK         = 0b00000000,
    SWITCHERROR      = 0b00000001,
    PRHOK            = 0b00000000,
    PRHERROR         = 0b00000100,
    SWITCHESACCEPTED = 0b01000000,
    CHARSOVERRUN     = 0b10000000
};

/**
 * Implements a single packet of the SonarReturnData protocol of the DeltaT100.
 */
class SonarReturnDataPacket : public SensorMessage
{
  public:
    constexpr static std::size_t command_length_ = SRDP_PACKET_SIZE;

    SonarReturnDataPacket() : SensorMessage(command_length_)
    {
    }

    explicit SonarReturnDataPacket(uint8_t* data) : SensorMessage(data, command_length_)
    {
    }

    Mode getMode() const
    {
        if (data_[SRDP_MODE_OFFSET] == 0x55)
        {
            return Mode::IUX;
        }
        return Mode::IVX;
    }

    SerialStatus getSerialStatus() const
    {
        return static_cast<SerialStatus>(data_[SRDP_SERIAL_STATUS_OFFSET]);
    }

    uint8_t getPacketNumber() const
    {
        return data_[SRDP_PACKET_NR_OFFSET];
    }

    uint8_t getFirmwareVersion() const
    {
        return data_[SRDP_FIRMWARE_V_OFFSET];
    }

    Range getRange() const
    {
        return static_cast<Range>(data_[SRDP_RANGE_OFFSET]);
    }

    uint16_t getNumberOfDataBytes() const
    {
        return static_cast<uint16_t>((data_[SRDP_DATABYTES_HIGH] << 8) | data_[SRDP_DATABYTES_LOW]);
    }

    uint8_t getExternalTriggerStatus() const
    {
        return data_[SRDP_EXTERNAL_TRIGGER_STATUS_OFFSET];
    }

    /**
     * Gets internal sensor status, only when packetnumber is 0.
     * 0 = No sensor installed
     * 1 = PRH Sensor Installed (837A)
     * 2 = PRH Sensor Installed (837B, signs are reversed)
     * 3 = Reserved
     * 4 = Reserved
     * 5 = PRH Sensor Installed (837)
     * 6 = Reserved
     * @return The internal sensor status.
     */
    uint8_t getInternalSensorStatus() const
    {
        return data_[SRDP_INTERAL_SENSOR_STATUS_OFFSET];
    }

    float getPitch() const
    {
        if ((data_[SRDP_PITCH_HIGH] & 0b10000000) == 0)
        {
            return static_cast<float>((data_[SRDP_PITCH_HIGH] << 8) | data_[SRDP_PITCH_LOW]) * SRDP_CONVERT_FACTOR;
        }
        else
        {
            return (static_cast<float>((data_[SRDP_PITCH_HIGH] << 8) | data_[SRDP_PITCH_LOW]) - 65536) * SRDP_CONVERT_FACTOR;
        }
    }

    float getRoll() const
    {
        if ((data_[SRDP_ROLL_HIGH] & 0b10000000) == 0)
        {
            return static_cast<float>((data_[SRDP_ROLL_HIGH] << 8) | data_[SRDP_ROLL_LOW]) * SRDP_CONVERT_FACTOR;
        }
        else
        {
            return (static_cast<float>((data_[SRDP_ROLL_HIGH] << 8) | data_[SRDP_ROLL_LOW]) - 65536) * SRDP_CONVERT_FACTOR;
        }
    }

    float getHeading() const
    {
        if ((data_[SRDP_HEADING_HIGH] & 0b10000000) == 0)
        {
            return static_cast<float>((data_[SRDP_HEADING_HIGH] << 8) | data_[SRDP_HEADING_LOW]) * SRDP_CONVERT_FACTOR;
        }
        else
        {
            return (static_cast<float>((data_[SRDP_HEADING_HIGH] << 8) | data_[SRDP_HEADING_LOW]) - 65536) * SRDP_CONVERT_FACTOR;
        }
    }

    uint16_t getTimerTicks() const
    {
        return static_cast<uint16_t>((data_[SRDP_TIMER_TICK_HIGH] << 8) | data_[SRDP_TIMER_TICK_LOW]);
    }

    RunMode getRunMode() const
    {
        return static_cast<RunMode>(data_[SRDP_RUNMODE_OFFSET]);
    }

    uint8_t getCurrentSensorGain() const
    {
        return data_[SRDP_SENSOR_GAIN_OFFSET];
    }

    uint16_t getAgcRange() const
    {
        return static_cast<uint16_t>((data_[SRDP_AGC_RANGE_HIGH] << 8) | data_[SRDP_AGC_RANGE_LOW]);
    }

    uint16_t getAgcMaximum() const
    {
        return static_cast<uint16_t>((data_[SRDP_AGC_MAX_HIGH] << 8) | data_[SRDP_AGC_MAX_LOW]);
    }

    std::pair<uint8_t*, uint16_t> getEchoData() const
    {
        return std::make_pair(&data_[SRDP_ECHO_DATA_OFFSET], getNumberOfDataBytes());
    }

    virtual ~SonarReturnDataPacket()
    {
    }
};

} /* namespace Delta100 */

constexpr Messages::SerialStatus operator&(Messages::SerialStatus lhs, Messages::SerialStatus rhs)
{
    using underlying = typename std::underlying_type<Messages::SerialStatus>::type;
    return static_cast<Messages::SerialStatus>(static_cast<underlying>(lhs) & static_cast<underlying>(rhs));
}

constexpr Messages::SerialStatus operator|(Messages::SerialStatus lhs, Messages::SerialStatus rhs)
{
    using underlying = typename std::underlying_type<Messages::SerialStatus>::type;
    return static_cast<Messages::SerialStatus>(static_cast<underlying>(lhs) | static_cast<underlying>(rhs));
}

#endif /* SRC_SONARRETURNDATAPACKET_HPP_ */
