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
        std::cout << __PRETTY_FUNCTION__ << std::endl;
    }

    explicit ProfilePointOutput(uint8_t* data, std::chrono::milliseconds::rep time = 0) : SensorMessage(data, command_length_, time)
    {
        std::cout << __PRETTY_FUNCTION__ << std::endl;
    }

    ProfilePointOutput(const ProfilePointOutput& rhs) : SensorMessage(rhs)
    {
        std::cout << __PRETTY_FUNCTION__ << std::endl;
    }

    virtual ~ProfilePointOutput()
    {
        std::cout << __PRETTY_FUNCTION__ << std::endl;
    }

    ProfilePointOutput& operator=(const ProfilePointOutput& rhs)
    {
        std::cout << __PRETTY_FUNCTION__ << std::endl;
        SensorMessage::operator=(rhs);
        return *this;
    }

    ProfilePointOutput& operator+=(const ProfilePointOutput& rhs)
    {
        uint16_t amount_of_beams          = getNumberOfBeams();
        std::size_t amount_zero           = 0;
        std::size_t amount_other_not_zero = 0;
        for (std::size_t i = 0; i < amount_of_beams; ++i)
        {
            if (getBeamRangeRaw(i) == 0 /* && rhs.getBeamRangeRaw(i) != 0*/)
            {
                amount_zero++;
            }
            if (rhs.getBeamRangeRaw(i) != 0)
            {
                data_[256 + i * 2] = rhs.getData()[256 + i * 2];
                data_[257 + i * 2] = rhs.getData()[257 + i * 2];
                amount_other_not_zero++;
            }
        }

        std::cout << "Amount 0: " << amount_zero << " beams: " << amount_of_beams
                  << " Assigned: " << amount_other_not_zero << std::endl;
        return *this;
    }

    uint16_t getTotalBytes() const
    {
        return ((data_[TOTAL_BYTES_HIGH] << 8) | data_[TOTAL_BYTES_LOW]);
    }

    uint16_t getNumberOfBeams() const
    {
        return ((data_[NR_OF_BEAMS_HIGH] << 8) | data_[NR_OF_BEAMS_LOW]);
    }

    uint16_t getSamplesPerBeam() const
    {
        return ((data_[SAMPLES_PER_BEAM_HIGH] << 8) | data_[SAMPLES_PER_BEAM_LOW]);
    }

    uint16_t getSectorSize() const
    {
        return ((data_[SECTOR_SIZE_HIGH] << 8) | data_[SECTOR_SIZE_LOW]);
    }

    // Beam 0 angle
    int16_t getStartAngle() const
    {
        return (((uint16_t)(data_[START_ANGLE_HIGH] << 8) | data_[START_ANGLE_LOW]) / 100) - 180;
    }

    // I assume this is a floating point value.
    float getAngleIncrement() const
    {
        return (float)data_[ANGLE_INCREMENT] / 100;
    }

    uint16_t getSoundVelocity() const
    {
        if ((data_[SOUND_VELOCITY_LOW] & 0x80) == 0)
            return 1500;
        return (((data_[SOUND_VELOCITY_LOW] & 0x7F) << 8) | data_[SOUND_VELOCITY_HIGHT]) / 10;
    }

    uint16_t getRangeResolution() const
    {
        return ((data_[RANGE_RESOLUTION_HIGH] << 8) | data_[RANGE_RESOLUTION_LOW]);
    }

    uint16_t getRepititionRate() const
    {
        return ((data_[REPETITION_RATE_HIGH] << 8) | data_[REPETITION_RATE_LOW]);
    }

    uint32_t getPingNumber() const
    {
        return (data_[PING_NUMBER_0] << 24) | (data_[PING_NUMBER_1] << 16) | (data_[PING_NUMBER_2] << 8) |
               data_[PING_NUMBER_3];
    }

    bool hasIntensity() const
    {
        return data_[INTENSITY_INCLUDED] == 1;
    }

    SampleRate getSampleRate() const
    {
        return data_[SAMPLE_RATE] == 0 ? SampleRate::STANDARD : SampleRate::HIGH;
    }

    uint8_t getAverageNumberOfPings() const
    {
        return data_[AVERAGE_PINGS];
    }

    float getBeamRange(uint16_t beam) const
    {
        if (!(beam < MAX_BEAMS))
        {
            return 0;
        }

        return getBeamRangeRaw(beam);
    }

    float getCorrectedBeamRange(uint16_t beam) const
    {
        if (!(beam < MAX_BEAMS))
        {
            return 0;
        }
        return getCorrectedBeamRangeRaw(beam);
    }

    std::array<float, 3> getBeamPosition(uint16_t beam) const
    {
        if (!(beam < MAX_BEAMS))
        {
            return std::array<float, 3>{ 0, 0, 0 };
        }
        float beam_range = getBeamRangeRaw(beam);
        float beam_angle = toRadians(getStartAngle() + getAngleIncrement() * beam);

        float x = beam_range * std::cos(-beam_angle);
        float y = beam_range * std::sin(beam_angle);
        // std::cout << "Beam range: " << std::setw(8) << beam_range << " angle: " << std::setw(8) << beam_angle << " x:
        // " << std::setw(6) << x << " z: " << std::setw(6) << z << std::endl;
        return std::array<float, 3>{ x, beam_range, y };
    }

    Matrix<MAX_BEAMS, 3, float> asMatrix() const
    {
        uint16_t number_of_beams = getNumberOfBeams();
        Matrix<MAX_BEAMS, 3, float> matrix;
        for (std::size_t i = 0; i < number_of_beams; ++i)
        {
            std::array<float, 3> beam_pos = getBeamPosition(i);
            matrix.at(i, 0)               = beam_pos.at(0);
            matrix.at(i, 1)               = beam_pos.at(0);
            matrix.at(i, 2)               = beam_pos.at(0);
        }

        return matrix;
        //	    	std::cout << matrix << std::endl;
    }

  private:
    float getBeamRangeRaw(uint16_t beam) const
    {
        return ((data_[PROFILE_RANGE_START_HIGH + beam * 2] << 8) | data_[PROFILE_RANGE_START_LOW + beam * 2]) *
               getRangeResolution() / 1000;
    }

    float getCorrectedBeamRangeRaw(uint16_t beam) const
    {
        return getBeamRangeRaw(beam) * getSoundVelocity() / 1500;
    }

    //    void setBeamRange(uint16_t beam, float range, float resolution = getRangeResolution())
    //    {
    //    	uint16_t converted_range = range / resolution * 1000;
    //    	data_[PROFILE_RANGE_START_HIGH + beam * 2] = (converted_range >> 8);
    //    	data_[PROFILE_RANGE_START_LOW + beam * 2] = (converted_range & 0xFF);
    //    }
};
}

#endif /* SRC_PROFILEPOINTOUTPUT_HPP_ */
