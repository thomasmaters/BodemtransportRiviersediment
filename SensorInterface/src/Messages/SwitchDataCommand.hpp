/*
 * SwitchDataCommand.hpp
 *
 *  Created on: 11 Jun 2018
 *      Author: Thomas Maters
 *		Email : thomasmaters@hotmail.com (TG.Maters@student.han.nl)
 */

#ifndef SRC_SWITCHDATACOMMAND_HPP_
#define SRC_SWITCHDATACOMMAND_HPP_

#include "SensorMessage.hpp"

#include <cstdint>

namespace Messages
{
enum class Mode : uint8_t
{
    IUX = 8,
    IVX = 16
};

enum class Range : uint8_t
{
    M5   = 5,
    M10  = 10,
    M20  = 20,
    M30  = 30,
    M40  = 40,
    M50  = 50,
    M60  = 60,
    M80  = 80,
    M100 = 100,
    M150 = 150,
    M200 = 200,
    M250 = 201,
    M300 = 202,
};

enum class RunMode : uint8_t
{
    XMITDISABLE     = 0b00000001,
    XMITENABLE      = 0b00000000,
    TVGDISABLE      = 0b00000010,
    TVGENABLE       = 0b00000000,
    AUTOGAINENABLE  = 0b00010000,
    AUTOGAINDISABLE = 0b00000000,

    ENABLEALL  = 0b00010011,
    DISABLEALL = 0b00000000
};

#define SDC_HEADER_SIZE 27
#define SDC_HEADER_BYTE_1 0xFE
#define SDC_HEADER_BYTE_2 0x44
#define SDC_INTERNAL_USE_9 1
#define SDC_HEAD_ID 0x10
#define SDC_TERMINATION_BYTE 0xFD

#define SDC_HEADER_BYTE_1_OFFSET 0
#define SDC_HEADER_BYTE_2_OFFSET 1
#define SDC_HEAD_ID_OFFSET 2
#define SDC_RANGE_OFFSET 3
#define SDC_NADIR_ANGLE_HIGH 5
#define SDC_NADIR_ANGLE_LOW 6
#define SDC_START_GAIN_OFFSET 8
#define SDC_INTERNAL_USE_9_OFFSET 9
#define SDC_ABSORPTION_OFFSET 10
#define SDC_AGC_THRESHOLD_OFFSET 11
#define SDC_PACKET_NR_REQEUST_OFFSET 13
#define SDC_PULSE_LENGTH_OFFSET 14
#define SDC_EX_TRIGGER_CONTROL_OFFSET 16
#define SDC_EX_TRIGGER_DELAY_HIGH 17
#define SDC_EX_TRIGGER_DELAY_LOW 18
#define SDC_DATA_POINTS_OFFSET 19
#define SDC_DATA_BITS_OFFSET 20
#define SDC_PRH_COMMAND_OFFSET 21
#define SDC_RUN_MODE_OFFSET 22
#define SDC_SWITCH_DELAY_OFFSET 24
#define SDC_FREQUENTY_OFFSET 25
#define SDC_TER_BYTE_OFFSET 26

/**
 * Class representing a SwitchDataCommand of the DeltaT100.
 */
class SwitchDataCommand : public SensorMessage
{
  public:
    constexpr static std::size_t command_length_ = SDC_HEADER_SIZE;

    enum class PulseLength : uint8_t
    {
        M5   = 3,
        M10  = 6,
        M20  = 12,
        M30  = 18,
        M40  = 24,
        M50  = 30,
        M60  = 36,
        M80  = 48,
        M100 = 60,
        M150 = 90,
        M200 = 120,
        M250 = 150,
        M300 = 180
    };

    enum class ExternalTriggerControlEdge : uint8_t
    {
        NEG = 0,
        POS = 1
    };

    enum class DataBits : uint8_t
    {
        BITS8 = 8
    };

    enum class PrhCommand : uint8_t
    {
        NOPRH                     = 0x00,
        STARTCOMPASSCALIBRATION   = 0x02,
        STOPCOMPASSCALIBRATION    = 0x03,
        STARTPITCHROLLCALIBRATION = 0x04,
        STOPPITCHROLLCALIBRATION  = 0x05,
        OUTPUTGYROEULERANGLES     = 0x80
    };

    enum class Frequency : uint8_t
    {
        KHZ120  = 58,
        KHZ260  = 86,
        KHZ675  = 169,
        KHZ1700 = 68
    };

    SwitchDataCommand() : SensorMessage(command_length_)
    {
        data_[SDC_HEADER_BYTE_1_OFFSET] = SDC_HEADER_BYTE_1;
        data_[SDC_HEADER_BYTE_2_OFFSET] = SDC_HEADER_BYTE_2;
        data_[SDC_HEAD_ID_OFFSET] = SDC_HEAD_ID;
        setRange(Range::M5);
        setNadirOffsetAngle(0);
        setStartGain(30);  // Gain
        data_[SDC_INTERNAL_USE_9_OFFSET] = SDC_INTERNAL_USE_9;
        setAbsorption(20);     // Absorption
        setAgcThreshold(120);  // AgcThreshold
        setPacketNumberRequest(0);        // PacketNumberRequest
        setPulseLength(PulseLength::M5);  // PulseLength
        enableExternalTriggerControl(false);
        setExternalTriggerControlEdge(ExternalTriggerControlEdge::NEG);
        setExternalTransmitDelay(0);
        setDataPoints(Mode::IUX);          // DataPoints
        setDataBits(DataBits::BITS8);      // DataBits
        setPrhCommand(PrhCommand::NOPRH);  // PrhCommand
        setRunMode(RunMode::DISABLEALL);   // RunMode
        setFrequency(Frequency::KHZ675);  // Frequency
        data_[SDC_TER_BYTE_OFFSET] = SDC_TERMINATION_BYTE;
    }

    virtual ~SwitchDataCommand()
    {
    }

    void setRange(Messages::Range value)
    {
        data_[SDC_RANGE_OFFSET] = static_cast<uint8_t>(value);
    }

    void setNadirOffsetAngle(int16_t value)
    {
        int16_t offsetAngle = value / 360 * 65536;
        if (value < 0)
        {
            offsetAngle |= 0x8000;
        }
        data_[SDC_NADIR_ANGLE_HIGH] = (offsetAngle & 0xFF00) >> 8;
        data_[SDC_NADIR_ANGLE_LOW] = offsetAngle & 0x00FF;
    }

    void setStartGain(uint8_t value)
    {
        if (!(value <= 30))
        {
            throw std::runtime_error("Gain not between valid values.[0-20]");
        }
        data_[SDC_START_GAIN_OFFSET] = value;
    }

    void setAbsorption(uint8_t value)
    {
        data_[SDC_ABSORPTION_OFFSET] = value;
    }

    void setAgcThreshold(uint8_t value)
    {
        if (!(value >= 10 && value <= 250))
        {
            throw std::runtime_error("AgcThreshold not between valid values.[10-250]");
        }
        data_[SDC_AGC_THRESHOLD_OFFSET] = value;
    }

    void setPacketNumberRequest(uint8_t value)
    {
        if (getMode() == static_cast<std::underlying_type<Mode>::type>(Mode::IUX))  // IUX mode
        {
            if (value < static_cast<std::underlying_type<Mode>::type>(Mode::IUX))
            {
                data_[SDC_PACKET_NR_REQEUST_OFFSET] = value;
            }
        }
        else if (getMode() == static_cast<std::underlying_type<Mode>::type>(Mode::IVX))
        {
            if (value <= static_cast<std::underlying_type<Mode>::type>(Mode::IVX))
            {
                data_[SDC_PACKET_NR_REQEUST_OFFSET] = value;
            }
        }
    }

    void setPulseLength(uint8_t value)
    {
        data_[SDC_PULSE_LENGTH_OFFSET] = value;
    }

    void setPulseLength(const PulseLength& value)
    {
        data_[SDC_PULSE_LENGTH_OFFSET] = static_cast<uint8_t>(value);
    }

    void setExternalTriggerControlEdge(const ExternalTriggerControlEdge& value)
    {
        data_[SDC_EX_TRIGGER_CONTROL_OFFSET] |= static_cast<uint8_t>(value);
    }

    void enableExternalTriggerControl(bool enable)
    {
        data_[SDC_EX_TRIGGER_CONTROL_OFFSET] ^= (-enable ^ data_[SDC_EX_TRIGGER_CONTROL_OFFSET]) & (1UL << 1);
    }

    void setExternalTransmitDelay(uint16_t value)
    {
        if (!(value % 100 == 0))
        {
            throw std::runtime_error("TransmitDelay not in steps of 100.");
        }
        if (!(value <= 10000))
        {
            throw std::runtime_error("ExternalTransmitDelay not in a valid range.[0-100000]");
        }

        data_[SDC_EX_TRIGGER_DELAY_HIGH] = (value >> 8) & 0xff;
        data_[SDC_EX_TRIGGER_DELAY_LOW] = value & 0xff;
    }

    void setDataPoints(const Mode& value)
    {
        data_[SDC_DATA_POINTS_OFFSET] = static_cast<std::underlying_type<Mode>::type>(value);
    }

    /**
     * Uses SDC_DATA_POINTS_OFFSET to get the mode.
     * @return The mode of this message.
     */
    Mode getMode() const
    {
    	return static_cast<Mode>(data_[SDC_DATA_POINTS_OFFSET]);
    }

    void setDataBits(uint8_t value = static_cast<std::underlying_type<Mode>::type>(DataBits::BITS8))
    {
        data_[SDC_DATA_BITS_OFFSET] = value;
    }

    void setDataBits(const DataBits& value)
    {
        data_[SDC_DATA_BITS_OFFSET] = static_cast<std::underlying_type<Mode>::type>(value);
    }

    void setPrhCommand(const PrhCommand& value)
    {
        data_[SDC_PRH_COMMAND_OFFSET] = static_cast<std::underlying_type<Mode>::type>(value);
    }

    void setRunMode(const RunMode& value)
    {
        data_[SDC_RUN_MODE_OFFSET] = static_cast<std::underlying_type<Mode>::type>(value);
    }

	/**
	 * Sets the switch delay from a uint16_t.
	 * @param value
	 */
    void setSwitchDelay(uint16_t value)
    {
        uint8_t aValue = value / 2;
        data_[SDC_SWITCH_DELAY_OFFSET]      = aValue;
    }

    /**
     * Sets the switch delay directly from the byte value.
     * @param value
     */
    void setSwitchDelay(uint8_t value)
    {
        data_[SDC_SWITCH_DELAY_OFFSET] = value;
    }

    void setFrequency(const Frequency& value)
    {
        data_[SDC_FREQUENTY_OFFSET] = static_cast<std::underlying_type<Mode>::type>(value);
    }
};

} /* namespace Messages */

constexpr Messages::RunMode operator|(Messages::RunMode lhs, Messages::RunMode rhs)
{
    using underlying = typename std::underlying_type<Messages::RunMode>::type;
    return static_cast<Messages::RunMode>(static_cast<underlying>(lhs) | static_cast<underlying>(rhs));
}

constexpr Messages::RunMode operator&(Messages::RunMode lhs, Messages::RunMode rhs)
{
    using underlying = typename std::underlying_type<Messages::RunMode>::type;
    return static_cast<Messages::RunMode>(static_cast<underlying>(lhs) & static_cast<underlying>(rhs));
}

#endif /* SRC_SWITCHDATACOMMAND_HPP_ */
