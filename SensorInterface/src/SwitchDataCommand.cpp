/*
 * SwitchDataCommand.cpp
 *
 *  Created on: 11 Jun 2018
 *      Author: Thomas Maters
 *		Email : thomasmaters@hotmail.com (TG.Maters@student.han.nl)
 */

#include "SwitchDataCommand.hpp"

namespace Delta100
{

SwitchDataCommand::SwitchDataCommand() : SensorMessage(27)
{
	data[0] = 0xFE;
	data[1] = 0x44;
	data[2] = 0x10; //Are there multiple heads?
	//Range
	data[4] = 0;
	//Nadir
	//Nadir
	data[7] = 0;
	//Gain
	data[9] = 1;
	//Absorption
	//AgcThreshold
	data[12] = 0;
	//PacketNumberRequest
	//PulseLength
	data[15] = 0;
	data[16] = 0;//ExternalTriggerControl //User defined default value
	//ExternalTransmitDelay
	//ExternalTransmitDelay
	//DataPoints
	//DataBits
	//PrhCommand
	//RunMode
	data[23] = 0;
	//SwitchDelay
	//Frequency
	data[26] = 0xFD;
}

SwitchDataCommand::~SwitchDataCommand()
{
}

} /* namespace Delta100 */
