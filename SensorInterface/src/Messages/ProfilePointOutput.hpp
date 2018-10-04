/*
 * ProfilePointOutput.hpp
 *
 *  Created on: 10 Jul 2018
 *      Author: Thomas Maters
 *		Email : thomasmaters@hotmail.com (TG.Maters@student.han.nl)
 */

#ifndef SRC_PROFILEPOINTOUTPUT_HPP_
#define SRC_PROFILEPOINTOUTPUT_HPP_

#include "../Profiler/Matrix.hpp"
#include "SensorMessage.hpp"

#define PPO_DEBUG 0
#if PPO_DEBUG > 0
#include <iostream>
#endif

#define PPO_TOTAL_BYTES_LOW 5
#define PPO_TOTAL_BYTES_HIGH 4
#define PPO_NR_OF_BEAMS_LOW 71
#define PPO_NR_OF_BEAMS_HIGH 70
#define PPO_SAMPLES_PER_BEAM_LOW 73
#define PPO_SAMPLES_PER_BEAM_HIGH 72
#define PPO_SECTOR_SIZE_LOW 75
#define PPO_SECTOR_SIZE_HIGH 74
#define PPO_START_ANGLE_LOW 77
#define PPO_START_ANGLE_HIGH 76
#define PPO_ANGLE_INCREMENT 78
#define PPO_SOUND_VELOCITY_LOW 84
#define PPO_SOUND_VELOCITY_HIGHT 83
#define PPO_RANGE_RESOLUTION_LOW 86
#define PPO_RANGE_RESOLUTION_HIGH 85
#define PPO_REPETITION_RATE_LOW 92
#define PPO_REPETITION_RATE_HIGH 91
#define PPO_PING_NUMBER_0 93
#define PPO_PING_NUMBER_1 94
#define PPO_PING_NUMBER_2 95
#define PPO_PING_NUMBER_3 96
#define PPO_INTENSITY_INCLUDED 117
#define PPO_SAMPLE_RATE 122
#define PPO_AVERAGE_PINGS 125
#define PPO_PROFILE_RANGE_START_LOW 257
#define PPO_PROFILE_RANGE_START_HIGH 256

#define PPO_BEAM_DATA_START 255
#define PPO_MAX_BEAMS 480
#define PPO_SOUND_VELOCITY 1500

namespace Messages
{
enum class SampleRate : uint16_t
{
    STANDARD = 500,
    HIGH     = 5000
};

/**
 * Class that used DeltaT.exe for converting sensordata to x,y,z data.
 */
class ProfilePointOutput : public SensorMessage
{
  public:
    constexpr static std::size_t command_length_ = PPO_BEAM_DATA_START + 4 * PPO_MAX_BEAMS;
    constexpr static float PI                    = std::acos(-1);
    constexpr static float toRadians(float degrees)
    {
        return degrees * PI / 180;
    }

    constexpr static float toDegrees(float radians)
    {
        return radians * 180 / PI;
    }

    ProfilePointOutput() : SensorMessage(command_length_)
    {
    }

    explicit ProfilePointOutput(uint8_t* data, std::chrono::milliseconds::rep time = 0)
      : SensorMessage(data, command_length_, time)
    {
    }

    ProfilePointOutput(const ProfilePointOutput& rhs) : SensorMessage(rhs)
    {
    }

    virtual ~ProfilePointOutput()
    {
    }

    ProfilePointOutput& operator=(const ProfilePointOutput& rhs)
    {
        SensorMessage::operator=(rhs);
        return *this;
    }

    ProfilePointOutput& operator+=(const ProfilePointOutput& rhs)
    {
        uint16_t amount_of_beams = getNumberOfBeams();
#if PPO_DEBUG > 0
        std::size_t amount_zero           = 0;
        std::size_t amount_other_not_zero = 0;
#endif
        for (std::size_t i = 0; i < amount_of_beams; ++i)
        {
#if PPO_DEBUG > 0
            if (getBeamRangeRaw(i) == 0 /* && rhs.getBeamRangeRaw(i) != 0*/)
            {
                amount_zero++;
            }
#endif
            if (rhs.getBeamRangeRaw(i) != 0)
            {
                data_[PPO_PROFILE_RANGE_START_HIGH + i * 2] = rhs.getData()[PPO_PROFILE_RANGE_START_HIGH + i * 2];
                data_[PPO_PROFILE_RANGE_START_LOW + i * 2]  = rhs.getData()[PPO_PROFILE_RANGE_START_LOW + i * 2];
#if PPO_DEBUG > 0
                amount_other_not_zero++;
#endif
            }
        }

#if PPO_DEBUG > 0
        std::cout << "Amount 0: " << amount_zero << " beams: " << amount_of_beams
                  << " Assigned: " << amount_other_not_zero << std::endl;
#endif
        return *this;
    }

    uint16_t getTotalBytes() const
    {
        return static_cast<uint16_t>((data_[PPO_TOTAL_BYTES_HIGH] << 8) | data_[PPO_TOTAL_BYTES_LOW]);
    }

    uint16_t getNumberOfBeams() const
    {
        return static_cast<uint16_t>((data_[PPO_NR_OF_BEAMS_HIGH] << 8) | data_[PPO_NR_OF_BEAMS_LOW]);
    }

    uint16_t getSamplesPerBeam() const
    {
        return static_cast<uint16_t>((data_[PPO_SAMPLES_PER_BEAM_HIGH] << 8) | data_[PPO_SAMPLES_PER_BEAM_LOW]);
    }

    uint16_t getSectorSize() const
    {
        return static_cast<uint16_t>((data_[PPO_SECTOR_SIZE_HIGH] << 8) | data_[PPO_SECTOR_SIZE_LOW]);
    }

    /**
     * Gets the start angle of beam 0.
     * @return
     */
    int16_t getStartAngle() const
    {
        return static_cast<uint16_t>((data_[PPO_START_ANGLE_HIGH] << 8) | data_[PPO_START_ANGLE_LOW]) / 100 - 180;
    }

    /**
     * Gets the spacing between beams.
     * @return
     */
    float getAngleIncrement() const
    {
        return static_cast<float>(data_[PPO_ANGLE_INCREMENT]) / 100;
    }

    uint16_t getSoundVelocity() const
    {
        if ((data_[PPO_SOUND_VELOCITY_LOW] & 0x80) == 0)
            return PPO_SOUND_VELOCITY;
        return static_cast<uint16_t>(((data_[PPO_SOUND_VELOCITY_LOW] & 0x7F) << 8) | data_[PPO_SOUND_VELOCITY_HIGHT]) /
               10;
    }

    uint16_t getRangeResolution() const
    {
        return static_cast<uint16_t>((data_[PPO_RANGE_RESOLUTION_HIGH] << 8) | data_[PPO_RANGE_RESOLUTION_LOW]);
    }

    uint16_t getRepititionRate() const
    {
        return static_cast<uint16_t>((data_[PPO_REPETITION_RATE_HIGH] << 8) | data_[PPO_REPETITION_RATE_LOW]);
    }

    uint32_t getPingNumber() const
    {
        return static_cast<uint32_t>((data_[PPO_PING_NUMBER_0] << 24) | (data_[PPO_PING_NUMBER_1] << 16) |
                                     (data_[PPO_PING_NUMBER_2] << 8) | data_[PPO_PING_NUMBER_3]);
    }

    bool hasIntensity() const
    {
        return data_[PPO_INTENSITY_INCLUDED] == 1;
    }

    SampleRate getSampleRate() const
    {
        return data_[PPO_SAMPLE_RATE] == 0 ? SampleRate::STANDARD : SampleRate::HIGH;
    }

    uint8_t getAverageNumberOfPings() const
    {
        return data_[PPO_AVERAGE_PINGS];
    }

    /**
     * Gets the beams length.
     * @param beam To get length of.
     * @return Length of the beam.
     */
    float getBeamRange(uint16_t beam) const
    {
        assert(beam >= PPO_MAX_BEAMS);

        return getBeamRangeRaw(beam);
    }

    /**
     * Gets the beam length corrected by the sound velocity.
     * @param beam To get length of.
     * @return Length of the beam.
     */
    float getCorrectedBeamRange(uint16_t beam) const
    {
        assert(beam >= PPO_MAX_BEAMS);

        return getCorrectedBeamRangeRaw(beam);
    }

    /**
     * Gets the x,y,beam length position of a beam.
     * @param beam Beam to get the position of.
     * @return Array with x,y,beam length value.
     */
    std::array<float, 3> getBeamPosition(uint16_t beam) const
    {
        if (beam >= PPO_MAX_BEAMS)
        {
            return std::array<float, 3>{ 0, 0, 0 };
        }
        float beam_range = getBeamRangeRaw(beam);
        float beam_angle = toRadians(getStartAngle() + getAngleIncrement() * beam);

        float x = beam_range * std::cos(-beam_angle);
        float y = beam_range * std::sin(-beam_angle);
        return std::array<float, 3>{ x, y, beam_range };
    }

    /**
     * Converts all the beams in a matrix representing the whole measurement.
     * @return
     */
    Matrix<PPO_MAX_BEAMS, 3, float> asMatrix() const
    {
        uint16_t number_of_beams = getNumberOfBeams();
        Matrix<PPO_MAX_BEAMS, 3, float> matrix;
        for (std::size_t i = 0; i < number_of_beams; ++i)
        {
            std::array<float, 3> beam_pos = getBeamPosition(i);
            matrix.at(i, 0)               = beam_pos.at(0);
            matrix.at(i, 1)               = beam_pos.at(1);
            matrix.at(i, 2)               = beam_pos.at(2);
        }

        return matrix;
    }

  private:
    /**
     * Gets the length of a beam (without checking the requested beam).
     * @param beam To get the length of.
     * @return Beam length.
     */
    float getBeamRangeRaw(uint16_t beam) const
    {
        return static_cast<float>((data_[PPO_PROFILE_RANGE_START_HIGH + beam * 2] << 8) |
                                     data_[PPO_PROFILE_RANGE_START_LOW + beam * 2]) *
               getRangeResolution() / 1000;
    }

    /**
     * Corrects the beamlength based on sound velocity.
     * @param beam To get.
     * @return Beam length corrected for the sound velocity.
     */
    float getCorrectedBeamRangeRaw(uint16_t beam) const
    {
        return getBeamRangeRaw(beam) * getSoundVelocity() / PPO_SOUND_VELOCITY;
    }
};
}

#endif /* SRC_PROFILEPOINTOUTPUT_HPP_ */
