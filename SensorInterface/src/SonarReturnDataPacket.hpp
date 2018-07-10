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

// Remove these includes:
#include <bitset>

namespace Controller::DeltaT100
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

/*
 *
 */
class SonarReturnDataPacket : public SensorMessage
{
  public:
    constexpr static std::size_t command_length_ = 1033;

    SonarReturnDataPacket() : SensorMessage(command_length_)
    {
    }

    SonarReturnDataPacket(uint8_t* data) : SensorMessage(data, command_length_)
    {
    }

    Mode getMode() const
    {
        if (data_[1] == 0x55)
        {
            return Mode::IUX;
        }
        return Mode::IVX;
    }

    SerialStatus getSerialStatus() const
    {
        return static_cast<SerialStatus>(data_[4]);
    }

    uint8_t getPacketNumber() const
    {
        return data_[5];
    }

    uint8_t getFirmwareVersion() const
    {
        return data_[6];  // TODO should this be parsed as an enum?
    }

    Range getRange()
    {
        return static_cast<Range>(data_[7]);
    }

    uint16_t getNumberOfDataBytes()
    {
        return (data_[10] << 8) | data_[11];
    }

    uint8_t getExternalTriggerStatus()
    {
        return data_[12];  // TODO should this be parsed as an enum?
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
    uint8_t getInternalSensorStatus()
    {
        return data_[13];  // TODO should this be parsed as an enum?
    }

    uint16_t getPitch()
    {
        if ((data_[15] & 0b10000000) == 0)
        {
            return ((data_[15] << 8) | data_[14]) * 360 / 65536;
        }
        else
        {
            return (((data_[15] << 8) | data_[14]) - 65536) * 360 / 65536;
        }
    }

    uint16_t getRoll()
    {
        if ((data_[17] & 0b10000000) == 0)
        {
            return ((data_[17] << 8) | data_[16]) * 360 / 65536;
        }
        else
        {
            return (((data_[17] << 8) | data_[16]) - 65536) * 360 / 65536;
        }
    }

    uint16_t getHeading()
    {
        if ((data_[19] & 0b10000000) == 0)
        {
            return ((data_[19] << 8) | data_[18]) * 360 / 65536;
        }
        else
        {
            return (((data_[19] << 8) | data_[18]) - 65536) * 360 / 65536;
        }
    }

    uint16_t getTimerTicks()
    {
        return (data_[21] << 8) | data_[20];
    }

    RunMode getRunMode()
    {
        return static_cast<RunMode>(data_[22]);
    }

    uint8_t getCurrentSensorGain() const
    {
        return data_[24];  // TODO: Should this variable be made in a enum?
    }

    uint16_t getAgcRange()
    {
        return ((data_[25] << 8) | data_[26]);
    }

    uint16_t getAgcMaximum()
    {
        return ((data_[27] << 8) | data_[28]);
    }

    std::pair<uint8_t*, uint16_t> getEchoData()
    {
        return std::make_pair(&data_[32], getNumberOfDataBytes());
    }

    void toString()
    {
        std::cout << "Mode: " << (getMode() == Mode::IUX ? "IUX" : "IVX") << std::endl;
        std::cout << "SerialStatus: "
                  << std::bitset<8>(static_cast<std::underlying_type<SerialStatus>::type>(getSerialStatus()))
                  << std::endl;
        std::cout << "PacketNumber: " << std::to_string(getPacketNumber()) << std::endl;
        std::cout << "Internal sensor status: " << std::bitset<8>(getInternalSensorStatus()) << std::endl;
    }

    virtual ~SonarReturnDataPacket()
    {
    }
};

} /* namespace Delta100 */

constexpr Controller::DeltaT100::SerialStatus operator&(Controller::DeltaT100::SerialStatus lhs,
                                                        Controller::DeltaT100::SerialStatus rhs)
{
    using underlying = typename std::underlying_type<Controller::DeltaT100::SerialStatus>::type;
    return static_cast<Controller::DeltaT100::SerialStatus>(static_cast<underlying>(lhs) &
                                                            static_cast<underlying>(rhs));
}

constexpr Controller::DeltaT100::SerialStatus operator|(Controller::DeltaT100::SerialStatus lhs,
                                                        Controller::DeltaT100::SerialStatus rhs)
{
    using underlying = typename std::underlying_type<Controller::DeltaT100::SerialStatus>::type;
    return static_cast<Controller::DeltaT100::SerialStatus>(static_cast<underlying>(lhs) |
                                                            static_cast<underlying>(rhs));
}

#endif /* SRC_SONARRETURNDATAPACKET_HPP_ */
