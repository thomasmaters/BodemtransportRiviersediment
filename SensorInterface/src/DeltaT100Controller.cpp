/*
 * DeltaT100Controller.cpp
 *
 *  Created on: 13 Jun 2018
 *      Author: Thomas Maters
 *		Email : thomasmaters@hotmail.com (TG.Maters@student.han.nl)
 */

#include "DeltaT100Controller.hpp"

#include "IOHandler.hpp"
#include "SonarReturnData.hpp"
#include "SonarReturnDataPacket.hpp"

#include <iostream>
#include <string>
#include <thread>

namespace Controller::DeltaT100
{
DeltaT100Controller::DeltaT100Controller(const std::string& host,
                                         const std::string& local_port,
                                         const std::string& remote_port)
  : sensor_communication_(IOHandler::getInstance().getIOService(), host, local_port, remote_port),
    deltat_communication_(IOHandler::getInstance().getIOService(), "localhost", local_port, remote_port),
    display_gain_(20),
    current_display_gain_(0),
    depth_profiler_(DepthProfiler<480, float>()),
    data_buffer_(std::unique_ptr<DataBuffer<>>(new DataBuffer<>()))
{
    std::cout << __PRETTY_FUNCTION__ << std::endl;
    depth_profiler_.setTransportUpdateEnabled(true);

    deltat_communication_.addRequestHandler(std::shared_ptr<RequestHandler>(this));
    sensor_communication_.addResponseHandler(std::shared_ptr<ResponseHandler>(this));

    switch_data_command_ = SwitchDataCommand();
    sensor_communication_.sendRequest(switch_data_command_, SonarReturnDataPacket::command_length_, false);
}

void DeltaT100Controller::handleResponse(uint8_t* data, std::size_t length, std::chrono::milliseconds::rep)
{
    std::cout << __PRETTY_FUNCTION__ << std::endl;
    if (length == SonarReturnDataPacket::command_length_)
    {
        // TODO: should we check if we are in bounds of a SonarReturnData message. length == SonarReturnData::length?
        std::unique_ptr<uint8_t[]>& stored_data = data_buffer_->moveToBuffer(data, length);
        SonarReturnDataPacket sonar_data(&(stored_data.get()[0]));

        sonar_data.toString();

        if ((sonar_data.getSerialStatus() & SerialStatus::SWITCHOK) == SerialStatus::SWITCHOK)
        {
            Mode mode             = sonar_data.getMode();
            uint8_t packet_number = sonar_data.getPacketNumber();

            if ((mode == Mode::IUX && packet_number < static_cast<std::underlying_type<Mode>::type>(Mode::IUX) - 1) ||
                (mode == Mode::IVX && packet_number < static_cast<std::underlying_type<Mode>::type>(Mode::IVX) - 1))
            {
                switch_data_command_.setPacketNumberRequest(packet_number + 1);
                sensor_communication_.sendRequest(switch_data_command_, SonarReturnDataPacket::command_length_, false);
            }
            else
            {
                switch_data_command_.setPacketNumberRequest(0);
                cosntructSensorPing();
            }
        }
        else
        {
            std::cout << "We hebben een error! Switches niet accepted." << std::endl;
            // TODO: Error!!!
        }
    }
    else
    {
        std::cout << "Length mismatch expected: " << std::to_string(SonarReturnDataPacket::command_length_)
                  << " got: " << std::to_string(length) << " Resending last package." << std::endl;
        sensor_communication_.sendRequest(switch_data_command_, SonarReturnDataPacket::command_length_, false);
    }
}

void DeltaT100Controller::cosntructSensorPing()
{
    // Construct a ping by moving the buffer into the ping.
    SonarReturnData ping(data_buffer_);
    data_buffer_ = std::unique_ptr<DataBuffer<>>(new DataBuffer<>());
    sensor_communication_.sendRequest(switch_data_command_, SonarReturnDataPacket::command_length_, false);
}

SensorMessage DeltaT100Controller::handleRequest(uint8_t* data, std::size_t length, std::chrono::milliseconds::rep time)
{
    std::cout << __PRETTY_FUNCTION__ << ": " << length << std::endl;
    if (length == ProfilePointOutput::command_length_)
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
            depth_profiler_.addProcessedPoint(matrix, time);
            profile_point_output_ = sonar_data;
            current_display_gain_ = 0;
        }

        // Return a message with 1 byte to reopen a connection.
        return SensorMessage(1);
    }
    else
    {
        std::ifstream matrixInput("C:\\Projecten\\Eclipse-workspace\\MultibeamDataProcessor\\Debug\\output_ssv.txt");
        Matrix<480, 3, float> test(matrixInput, false);
        current_display_gain_++;
        Matrix<480, 3, float> temp;
        for (std::size_t i = 0; i < 479; ++i)
        {
        	temp.at(i,0) = current_display_gain_;
//        	temp.at(i, 0) = test.at(i,0);
//            temp.at((i + current_display_gain_) % 479 , 1) = test.at(i,1);
        }
        test +=temp;
        depth_profiler_.addRawPoint(test, time);
    }

    // Return an empty message to stop the connection.
    return SensorMessage(0);
}

void DeltaT100Controller::requestSensorPing(const SwitchDataCommand& command)
{
    sensor_communication_.sendRequest(
        dynamic_cast<const SensorMessage&>(command), SonarReturnDataPacket::command_length_, false);
}

DeltaT100Controller::~DeltaT100Controller()
{
}

} /* namespace Controller */
