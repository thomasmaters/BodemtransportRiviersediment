/*
 * SonarReturnData.hpp
 *
 *  Created on: 11 Jun 2018
 *      Author: Thomas Maters
 *		Email : thomasmaters@hotmail.com (TG.Maters@student.han.nl)
 */

#ifndef SRC_SONARRETURNDATA_HPP_
#define SRC_SONARRETURNDATA_HPP_

#include <cstdint>
#include <memory>
#include "SwitchDataCommand.hpp"
#include "SensorMessage.hpp"

namespace Controller::DeltaT100
{
/*
 *
 */
class SonarReturnData : public SensorMessage
{
	enum class SerialStatus : uint8_t
	{
		SWITCHOK = 0b00000000,
		SWITCHERROR = 0b00000001,
		PRHOK = 0b00000000,
		PRHERROR = 0b00000100,
		SWITCHESACCEPTED = 0b01000000,
		CHARSOVERRUN = 0b10000000
	};

public:
	SonarReturnData() : SensorMessage(1032)
	{

	}

	Mode getMode() const
	{
		if(data_[1] == 0x55)
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
		return data_[6]; //TODO should this be parsed as an enum?
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
		return data_[12]; //TODO should this be parsed as an enum?
	}

	uint8_t getInternalSensorStatus()
	{
		return data_[13]; //TODO should this be parsed as an enum?
	}

	uint16_t getPitch()
	{
		if((data_[15] & 0b10000000) == 0)
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
		if((data_[17] & 0b10000000) == 0)
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
		if((data_[19] & 0b10000000) == 0)
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

	uint8_t getGain()
	{
		return data_[24];//TODO: Should this variable be made in a enum?
	}

	uint16_t getAgcRange()
	{
		return ((data_[25] << 8) | data_[26]);
	}

	uint16_t getAgcMaximum()
	{
		return ((data_[27] << 8) | data_[28]);
	}

	uint8_t* getEchoData()
	{
		return &data_[32];
	}

	virtual ~SonarReturnData()
	{

	}
};

} /* namespace Delta100 */

#endif /* SRC_SONARRETURNDATA_HPP_ */
