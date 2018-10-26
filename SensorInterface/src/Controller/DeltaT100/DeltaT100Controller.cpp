/*
 * DeltaT100Controller.cpp
 *
 *  Created on: 13 Jun 2018
 *      Author: Thomas Maters
 *		Email : thomasmaters@hotmail.com (TG.Maters@student.han.nl)
 */

#include "../../Controller/DeltaT100/DeltaT100Controller.hpp"
#include "../../Messages/SonarReturnData.hpp"
#include "../../Messages/SonarReturnDataPacket.hpp"

#include <fstream>

#if DELTAT100_DEBUG > 0
#include <iostream>
#endif

namespace Controller::DeltaT100
{
using namespace Communication;
DeltaT100Controller::DeltaT100Controller(const std::string& host,
                                         const std::string& remote_port,
                                         const std::string& local_port)
  : DeltaT100ControllerProxy(host, remote_port, local_port),
    display_gain_(DELTAT100_DISPLAY_GAIN),
    current_display_gain_(0),
    depth_profiler_(Profiler::DepthProfiler<DELTAT100_BEAM_COUNT, float>()),
    data_buffer_(std::unique_ptr<DataBuffer<>>(new DataBuffer<>()))
{
    depth_profiler_.setTransportUpdateEnabled(true);

    switch_data_command_ = SwitchDataCommand();
    requestSensorPing(switch_data_command_);
}

void DeltaT100Controller::handleResponse(uint8_t* data, std::size_t length, std::chrono::milliseconds::rep)
{
    // We received a SonarReturnDataPacket
    if (length == SonarReturnDataPacket::command_length_)
    {
        std::unique_ptr<uint8_t[]>& stored_data = data_buffer_->moveToBuffer(data, length);
        SonarReturnDataPacket sonar_data(&(stored_data.get()[0]));

        // Check status flags.
        if ((sonar_data.getSerialStatus() & SerialStatus::SWITCHOK) == SerialStatus::SWITCHOK)
        {
            Mode mode             = sonar_data.getMode();
            uint8_t packet_number = sonar_data.getPacketNumber();

            // Request next packet or a new packet.
            if ((mode == Mode::IUX && packet_number < static_cast<std::underlying_type<Mode>::type>(Mode::IUX) - 1) ||
                (mode == Mode::IVX && packet_number < static_cast<std::underlying_type<Mode>::type>(Mode::IVX) - 1))
            {
                switch_data_command_.setPacketNumberRequest(packet_number + 1);
                requestSensorPing(switch_data_command_);
            }
            else
            {
                switch_data_command_.setPacketNumberRequest(0);
                cosntructSensorPing();
            }
        }
        else
        {
            // Re-send request.
            requestSensorPing(switch_data_command_);
        }
    }
    else
    {
#if DELTAT100_DEBUG > 0
        std::cout << "Length mismatch expected: " << std::to_string(SonarReturnDataPacket::command_length_)
                  << " got: " << std::to_string(length) << " Resending last package." << std::endl;
#endif
        // Re-send request.
        requestSensorPing(switch_data_command_);
    }
}

void DeltaT100Controller::cosntructSensorPing()
{
    // Construct a ping by moving the buffer into the ping.
    SonarReturnData ping(data_buffer_);
    data_buffer_ = std::unique_ptr<DataBuffer<>>(new DataBuffer<>());
    requestSensorPing(switch_data_command_);
}

SensorMessage DeltaT100Controller::handleRequest(uint8_t* data, std::size_t length, std::chrono::milliseconds::rep time)
{
    // We received a ProfilePointOutput message from DeltaT.exe
    if (length == ProfilePointOutput::command_length_ || length == PPO_MIN_MESSAGE_SIZE)
    {
        ProfilePointOutput sonar_data(data);

        if (current_display_gain_ < display_gain_)
        {
            profile_point_output_ += sonar_data;
            current_display_gain_++;
        }
        else
        {
            auto matrix = profile_point_output_.asMatrix();
            // Activate calculation of transport.
            depth_profiler_.addProcessedPoint(matrix, time);
            profile_point_output_ = sonar_data;
            current_display_gain_ = 0;
        }

        // Return a message with 1 byte to reopen a connection.
        return SensorMessage(1);
    }
#if DELTAT100_DEBUG == 1
    else
    {
        // Simulate a sensormessage if we have none.
        std::ifstream matrixInput("C:\\Projecten\\Eclipse-workspace\\MultibeamDataProcessor\\Debug\\output_ssv.txt");
        Matrix<DELTAT100_BEAM_COUNT, 3, float> test(matrixInput, false);
        current_display_gain_++;
        Matrix<DELTAT100_BEAM_COUNT, 3, float> temp;
        for (std::size_t i = 0; i < DELTAT100_BEAM_COUNT - 1; ++i)
        {
            temp.at(i, 0) = current_display_gain_;
        }
        test += temp;
        depth_profiler_.addRawPoint(test, time);
    }
#endif

    // Return an empty message to stop the connection.
    return SensorMessage(0);
}

DeltaT100Controller::~DeltaT100Controller()
{
}

} /* namespace Controller */
