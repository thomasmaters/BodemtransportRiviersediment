/*
 * SwitchDataCommand.hpp
 *
 *  Created on: 11 Jun 2018
 *      Author: Thomas Maters
 *		Email : thomasmaters@hotmail.com (TG.Maters@student.han.nl)
 */

#ifndef SRC_SWITCHDATACOMMAND_HPP_
#define SRC_SWITCHDATACOMMAND_HPP_

#include <cstdint>
#include <stdexcept>

#include "SensorMessage.hpp"

namespace Controller::DeltaT100
{
enum class Mode : uint8_t
{
    IUX = 8,
    IVX = 16
};

enum class Range : uint8_t
{
    M5 = 5,
    M10 = 10,
    M20 = 20,
    M30 = 30,
    M40 = 40,
    M50 = 50,
    M60 = 60,
    M80 = 80,
    M100 = 100,
    M150 = 150,
    M200 = 200,
    M250 = 201,
    M300 = 202,
};

enum class RunMode : uint8_t
{
    XMITDISABLE = 0b00000001,
    XMITENABLE = 0b00000000,
    TVGDISABLE = 0b00000010,
    TVGENABLE = 0b00000000,
    AUTOGAINENABLE = 0b00010000,
    AUTOGAINDISABLE = 0b00000000,

    ENABLEALL = 0b00010011,
    DISABLEALL = 0b00000000
};

/*
 *
 */
class SwitchDataCommand : public SensorMessage
{
  public:
    enum class PulseLength : uint8_t
    {
        M5 = 3,
        M10 = 6,
        M20 = 12,
        M30 = 18,
        M40 = 24,
        M50 = 30,
        M60 = 36,
        M80 = 48,
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
        NOPRH = 0x00,
        STARTCOMPASSCALIBRATION = 0x02,
        STOPCOMPASSCALIBRATION = 0x03,
        STARTPITCHROLLCALIBRATION = 0x04,
        STOPPITCHROLLCALIBRATION = 0x05,
        OUTPUTGYROEULERANGLES = 0x80
    };

    enum class Frequency : uint8_t
    {
        KHZ120 = 58,
        KHZ260 = 86,
        KHZ675 = 169,
        KHZ1700 = 68
    };

    SwitchDataCommand();

    static SwitchDataCommand getDefaultInstance()
    {
    	SwitchDataCommand temp;
    	temp.setRange(Range::M30);
    	temp.setNadirOffsetAngle(0);
    	temp.setStartGain(10);
    	temp.setAbsorption(0);
    	temp.setAgcThreshold(10);
    	temp.setPacketNumberRequest(0);
    	temp.setPulseLength(PulseLength::M30);
    	temp.setExternalTriggerControlEdge(ExternalTriggerControlEdge::NEG);
    	temp.enableExternalTriggerControl(false);
    	temp.setExternalTransmitDelay(0);
    	temp.setDataPoints(Mode::IUX);
    	temp.setDataBits(DataBits::BITS8);
    	temp.setPrhCommand(PrhCommand::NOPRH);
    	temp.setRunMode(RunMode::DISABLEALL);
    	temp.setSwitchDelay((uint8_t)0);
    	temp.setFrequency(Frequency::KHZ120);
    	return temp;
    }

    void setRange(Controller::DeltaT100::Range value)
    {
    	data[3] = static_cast<uint8_t>(value);
    }

    void setNadirOffsetAngle(int16_t value)
    {
    	int16_t offsetAngle = value / 360 *65536;
    	if(value < 0){ offsetAngle |= 0x8000; }
    	data[5] = (offsetAngle & 0xFF00) >> 8;
    	data[6] = offsetAngle & 0x00FF;
    }

    void setStartGain(uint8_t value)
    {
        if (!(value >= 0 && value <= 20))
        {
            throw std::runtime_error("Gain not between valid values.[0-20]");
        }
        data[8] = value;
    }

    void setAbsorption(uint8_t value)
    {
        data[10] = value;
    }

    void setAgcThreshold(uint8_t value)
    {
        if (!(value >= 10 && value <= 250))
        {
            throw std::runtime_error("AgcThreshold not between valid values.[10-250]");
        }
        data[11] = value;
    }

    void setPacketNumberRequest(uint8_t value)
    {
        if (data[19] == static_cast<std::underlying_type<Mode>::type>(Mode::IUX))  // IUX mode
        {
            if (value <= 7)
            {
                data[13] = value;
            }
        }
        else if (data[19] == static_cast<std::underlying_type<Mode>::type>(Mode::IVX))
        {
            if (value <= 15)
            {
                data[13] = value;
            }
        }
    }

    void setPulseLength(uint8_t value)
    {
        data[14] = value;
    }

    void setPulseLength(const PulseLength& value)
    {
        data[14] = static_cast<uint8_t>(value);
    }

    void setExternalTriggerControlEdge(const ExternalTriggerControlEdge& value)
    {
        data[16] |= static_cast<uint8_t>(value);
    }

    void enableExternalTriggerControl(bool enable)
    {
        data[16] ^= (-enable ^ data[16]) & (1UL << 1);
    }

    void setExternalTransmitDelay(uint16_t value)
    {
        if (!(value % 100 == 0))
        {
            throw std::runtime_error("TransmitDelay not in steps of 100.");
        }
        if (!(value >= 0 && value <= 10000))
        {
            throw std::runtime_error("ExternalTransmitDelay not in a valid range.[0-100000]");
        }

        // TODO: how is the byte orderning? Has it to be flipped?
        data[17] = value & 0xff;
        data[18] = (value >> 8) & 0xff;
    }

    void setDataPoints(const Mode& value)
    {
        data[19] = static_cast<std::underlying_type<Mode>::type>(value);
    }

    void setDataBits(uint8_t value = static_cast<std::underlying_type<Mode>::type>(DataBits::BITS8))
    {
        data[20] = value;
    }

    void setDataBits(const DataBits& value)
    {
        data[20] = static_cast<std::underlying_type<Mode>::type>(value);
    }

    void setPrhCommand(const PrhCommand& value)
    {
        data[21] = static_cast<std::underlying_type<Mode>::type>(value);
    }

    void setRunMode(const RunMode& value)
    {
        data[22] = static_cast<std::underlying_type<Mode>::type>(value);
    }

    // Calculates the single byte value for the switch delay;
    void setSwitchDelay(uint16_t value)
    {
        uint8_t aValue = value / 2;
        if (!(aValue >= 0 && aValue <= UINT8_MAX))
        {
            throw std::runtime_error("SwitchDelay not in a valid range.[0-254]");
        }
        data[24] = aValue;
    }

    // Sets the switch delay directly to this value.
    void setSwitchDelay(uint8_t value)
    {
        data[24] = value;
    }

    void setFrequency(const Frequency& value)
    {
        data[25] = static_cast<std::underlying_type<Mode>::type>(value);
    }

    virtual ~SwitchDataCommand();
};

} /* namespace Delta100 */

constexpr Controller::DeltaT100::RunMode operator|(Controller::DeltaT100::RunMode lhs, Controller::DeltaT100::RunMode rhs)
{
    using underlying = typename std::underlying_type<Controller::DeltaT100::RunMode>::type;
    return static_cast<Controller::DeltaT100::RunMode>(static_cast<underlying>(lhs) | static_cast<underlying>(rhs));
}

constexpr Controller::DeltaT100::RunMode operator&(Controller::DeltaT100::RunMode lhs, Controller::DeltaT100::RunMode rhs)
{
    using underlying = typename std::underlying_type<Controller::DeltaT100::RunMode>::type;
    return static_cast<Controller::DeltaT100::RunMode>(static_cast<underlying>(lhs) & static_cast<underlying>(rhs));
}

#endif /* SRC_SWITCHDATACOMMAND_HPP_ */
