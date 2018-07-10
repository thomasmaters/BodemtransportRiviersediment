/*
 * ProfilePointOutput.hpp
 *
 *  Created on: 10 Jul 2018
 *      Author: Thomas Maters
 *		Email : thomasmaters@hotmail.com (TG.Maters@student.han.nl)
 */

#ifndef SRC_PROFILEPOINTOUTPUT_HPP_
#define SRC_PROFILEPOINTOUTPUT_HPP_

#include "SensorMessage.hpp"

#define TOTAL_BYTES_LOW 5
#define TOTAL_BYTES_HIGH 4
#define NR_OF_BEAMS_LOW 71
#define NR_OF_BEAMS_HIGH 70
#define SAMPLES_PER_BEAM_LOW 73
#define SAMPLES_PER_BEAM_HIGH 72
#define SECTOR_SIZE_LOW 75
#define SECTOR_SIZE_HIGH 74
#define START_ANGLE_LOW 77
#define START_ANGLE_HIGH 76
#define ANGLE_INCREMENT 78
#define SOUND_VELOCITY_LOW 84
#define SOUND_VELOCITY_HIGHT 83
#define REPETITION_RATE_LOW 92
#define REPETITION_RATE_HIGH 91
#define PING_NUMBER_0 93
#define PING_NUMBER_1 94
#define PING_NUMBER_2 95
#define PING_NUMBER_3 96
#define INTENSITY_INCLUDED 117
#define SAMPLE_RATE 122
#define AVERAGE_PINGS 125

namespace Controller::DeltaT100
{
	enum class SampleRate : uint16_t
	{
		STANDARD = 500,
		HIGH = 5000
	};

	class ProfilePointOutput : public SensorMessage
	{
	  public:
	    constexpr static std::size_t command_length_ = 255 + 4 * 480;

	    ProfilePointOutput() : SensorMessage(command_length_)
	    {
	    }

	    ProfilePointOutput(uint8_t* data) : SensorMessage(data, command_length_)
	    {
	    }

	    uint16_t getTotalBytes()
	    {
	    	return ((data_[TOTAL_BYTES_HIGH] << 8) | data_[TOTAL_BYTES_LOW]);
	    }

	    uint16_t getNumberOfBeams()
	    {
	    	return ((data_[NR_OF_BEAMS_HIGH] << 8) | data_[NR_OF_BEAMS_LOW]);
	    }

	    uint16_t getSamplesPerBeam()
	    {
	    	return ((data_[SAMPLES_PER_BEAM_HIGH] << 8) | data_[SAMPLES_PER_BEAM_LOW]);
	    }

	    uint16_t getSectorSize()
	    {
	    	return ((data_[SECTOR_SIZE_HIGH] << 8) | data_[SECTOR_SIZE_LOW]);
	    }

	    //Beam 0 angle
	    uint16_t getStartAngle()
	    {
	    	return (((data_[SAMPLES_PER_BEAM_HIGH] << 8) | data_[SAMPLES_PER_BEAM_LOW]) / 100) - 180;
	    }

	    //I assume this is a floating point value.
	    float getAngleIncrement()
	    {
	    	return data_[ANGLE_INCREMENT] / 100;
	    }

	    uint16_t getSoundVelocity()
	    {
	    	if((data_[SOUND_VELOCITY_LOW] & 0x80) == 0) return 1500;
	    	return (((data_[SOUND_VELOCITY_LOW] & 0x7F) << 8) | data_[SOUND_VELOCITY_HIGHT]) / 10;
	    }

	    uint16_t getRepititionRate()
	    {
	    	return ((data_[REPETITION_RATE_HIGH] << 8) | data_[REPETITION_RATE_LOW]);
	    }

	    uint32_t getPingNumber()
	    {
	    	return (data_[PING_NUMBER_0] << 24) | (data_[PING_NUMBER_1] << 16) | (data_[PING_NUMBER_2] << 8) | data_[PING_NUMBER_3];
	    }

	    bool hasIntensity()
	    {
	    	return data_[INTENSITY_INCLUDED] == 1;
	    }

	    SampleRate getSampleRate()
	    {
	    	return data_[SAMPLE_RATE] == 0 ? SampleRate::STANDARD : SampleRate::HIGH;
	    }

	    uint8_t getAverageNumberOfPings()
	    {
	    	return data_[AVERAGE_PINGS];
	    }
	};

}



#endif /* SRC_PROFILEPOINTOUTPUT_HPP_ */
