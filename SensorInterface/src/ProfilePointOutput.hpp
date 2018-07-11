/*
 * ProfilePointOutput.hpp
 *
 *  Created on: 10 Jul 2018
 *      Author: Thomas Maters
 *		Email : thomasmaters@hotmail.com (TG.Maters@student.han.nl)
 */

#ifndef SRC_PROFILEPOINTOUTPUT_HPP_
#define SRC_PROFILEPOINTOUTPUT_HPP_

#include "Matrix.hpp"
#include "SensorMessage.hpp"

#include <iomanip>

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
#define RANGE_RESOLUTION_LOW 86
#define RANGE_RESOLUTION_HIGH 85
#define REPETITION_RATE_LOW 92
#define REPETITION_RATE_HIGH 91
#define PING_NUMBER_0 93
#define PING_NUMBER_1 94
#define PING_NUMBER_2 95
#define PING_NUMBER_3 96
#define INTENSITY_INCLUDED 117
#define SAMPLE_RATE 122
#define AVERAGE_PINGS 125
#define PROFILE_RANGE_START_LOW 257
#define PROFILE_RANGE_START_HIGH 256

#define MAX_BEAMS 480

namespace Controller::DeltaT100
{
enum class SampleRate : uint16_t
{
    STANDARD = 500,
    HIGH     = 5000
};

class ProfilePointOutput : public SensorMessage
{
  public:
    constexpr static std::size_t command_length_ = 255 + 4 * MAX_BEAMS;
    constexpr static float PI                    = std::acos(-1);
    constexpr static float toRadians(float degrees)
    {
        return degrees * PI / 180;
    }

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

    // Beam 0 angle
    int16_t getStartAngle()
    {
        return (((uint16_t)(data_[START_ANGLE_HIGH] << 8) | data_[START_ANGLE_LOW]) / 100) - 180;
    }

    // I assume this is a floating point value.
    float getAngleIncrement()
    {
        return (float)data_[ANGLE_INCREMENT] / 100;
    }

    uint16_t getSoundVelocity()
    {
        if ((data_[SOUND_VELOCITY_LOW] & 0x80) == 0)
            return 1500;
        return (((data_[SOUND_VELOCITY_LOW] & 0x7F) << 8) | data_[SOUND_VELOCITY_HIGHT]) / 10;
    }

    uint16_t getRangeResolution()
    {
        return ((data_[RANGE_RESOLUTION_HIGH] << 8) | data_[RANGE_RESOLUTION_LOW]);
    }

    uint16_t getRepititionRate()
    {
        return ((data_[REPETITION_RATE_HIGH] << 8) | data_[REPETITION_RATE_LOW]);
    }

    uint32_t getPingNumber()
    {
        return (data_[PING_NUMBER_0] << 24) | (data_[PING_NUMBER_1] << 16) | (data_[PING_NUMBER_2] << 8) |
               data_[PING_NUMBER_3];
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

    float getBeamRange(uint16_t beam)
    {
        if (!(beam >= 0 && beam < MAX_BEAMS))
        {
            return 0;
        }

        return getBeamRangeRaw(beam);
    }

    float getCorrectedBeamRange(uint16_t beam)
    {
        if (!(beam >= 0 && beam < MAX_BEAMS))
        {
            return 0;
        }
        return getCorrectedBeamRangeRaw(beam);
    }

    std::array<float, 3> getBeamPosition(uint16_t beam)
    {
        if (!(beam >= 0 && beam < MAX_BEAMS))
        {
            return std::array<float, 3>{ 0, 0, 0 };
        }
        float beam_range = getCorrectedBeamRangeRaw(beam);
        float beam_angle = toRadians(getStartAngle() + getAngleIncrement() * beam);

        float z = beam_range * std::cos(beam_angle);
        float x = beam_range * std::sin(beam_angle);
        // std::cout << "Beam range: " << std::setw(8) << beam_range << " angle: " << std::setw(8) << beam_angle << " x:
        // " << std::setw(6) << x << " z: " << std::setw(6) << z << std::endl;
        return std::array<float, 3>{ x, 0, z };
    }

    Matrix<MAX_BEAMS, 3, float> asMatrix()
    {
        uint16_t number_of_beams = getNumberOfBeams();
        Matrix<MAX_BEAMS, 3, float> matrix;
        for (std::size_t i = 0; i < number_of_beams; ++i)
        {
            matrix.at(i) = getBeamPosition(i);
        }

        return matrix;
        //	    	std::cout << matrix << std::endl;
    }

  private:
    float getBeamRangeRaw(uint16_t beam)
    {
        return ((data_[PROFILE_RANGE_START_HIGH + beam * 2] << 8) | data_[PROFILE_RANGE_START_LOW + beam * 2]) *
               getRangeResolution() / 1000;
    }

    float getCorrectedBeamRangeRaw(uint16_t beam)
    {
        return getBeamRangeRaw(beam) * getSoundVelocity() / 1500;
    }
};
}

#endif /* SRC_PROFILEPOINTOUTPUT_HPP_ */
