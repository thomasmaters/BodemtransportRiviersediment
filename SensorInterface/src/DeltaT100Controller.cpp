/*
 * DeltaT100Controller.cpp
 *
 *  Created on: 13 Jun 2018
 *      Author: Thomas Maters
 *		Email : thomasmaters@hotmail.com (TG.Maters@student.han.nl)
 */

#include "DeltaT100Controller.hpp"

#include "SonarReturnData.hpp"
#include "DepthProfiler.hpp"
#include "DeltaT100Ping.hpp"

#include <iostream>
#include <random>
#include <string>
#include <thread>

namespace Controller::DeltaT100
{
DeltaT100Controller::DeltaT100Controller(boost::asio::io_service& io_service,
                                         const std::string& host,
                                         const std::string& local_port,
                                         const std::string& remote_port)
  : io_service_(io_service),
    sensor_communication_(io_service_, host, local_port, remote_port),
    data_buffer_(DataBuffer<>())
{
    std::cout << __PRETTY_FUNCTION__ << std::endl;
    sensor_communication_.addRequestHandler(std::shared_ptr<RequestHandler>(this));
    sensor_communication_.addResponseHandler(std::shared_ptr<ResponseHandler>(this));
    //    std::random_device rd;
    //    std::mt19937 generator(rd());
    //
    //    std::thread a(std::thread([&] {
    //        SensorMessage temp = Controller::DeltaT100::SwitchDataCommand::getDefaultInstance();
    //        sensor_communication_.sendRequest(temp, (std::size_t)5, true);
    //        std::this_thread::sleep_for(std::chrono::seconds(5));
    //        while (1)
    //        {
    //            if (true)
    //            {
    //                std::string str("0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz");
    //
    //                std::shuffle(str.begin(), str.end(), generator);
    //
    //                sensor_communication_.sendRequest(str.substr(0, 5), 10);
    //            }
    //            std::this_thread::sleep_for(std::chrono::seconds(15));
    //        }
    //    }));
    switch_data_command_ = SwitchDataCommand();
    sensor_communication_.sendRequest(switch_data_command_, SonarReturnData::command_length_, false);
    auto work = std::make_shared<boost::asio::io_service::work>(io_service_);
    io_service_.run();
    std::cout << "Preemptive exit" << std::endl;
    //    a.join();
}

void DeltaT100Controller::handleResponse(uint8_t* data, std::size_t length)
{
	std::cout << __PRETTY_FUNCTION__ << std::endl;
    if (length == SonarReturnData::command_length_)
    {
        // TODO: should we check if we are in bounds of a SonarReturnData message. length == SonarReturnData::length?
        std::unique_ptr<uint8_t[]>& stored_data = data_buffer_.moveToBuffer(data, length);
        SonarReturnData sonar_data(&(stored_data.get()[0]));

        sonar_data.toString();

        if ((sonar_data.getSerialStatus() & SerialStatus::SWITCHOK) == SerialStatus::SWITCHOK)
        {
            Mode mode             = sonar_data.getMode();
            uint8_t packet_number = sonar_data.getPacketNumber();

            if ((mode == Mode::IUX && packet_number < static_cast<std::underlying_type<Mode>::type>(Mode::IUX) - 1) ||
                (mode == Mode::IVX && packet_number < static_cast<std::underlying_type<Mode>::type>(Mode::IVX) - 1))
            {
                switch_data_command_.setPacketNumberRequest(packet_number + 1);
                sensor_communication_.sendRequest(switch_data_command_, SonarReturnData::command_length_, false);
            }
            else
            {
                switch_data_command_.setPacketNumberRequest(0);
                cosntructSensorPing(mode);
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
    	std::cout << "Length mismatch expected: " << std::to_string(SonarReturnData::command_length_) << " got: " << std::to_string(length) << std::endl;
    }
}

void DeltaT100Controller::cosntructSensorPing(Mode mode)
{
    std::cout << "RECEIVED A PING" << std::endl;
    DeltaT100Ping ping(data_buffer_, (Mode)switch_data_command_.getData()[19]);
    DepthProfiler::getInstance().addSensorPing(ping);
    data_buffer_.clear();
    sensor_communication_.sendRequest(switch_data_command_, SonarReturnData::command_length_, false);
}

SensorMessage DeltaT100Controller::handleRequest(uint8_t* data, std::size_t length)
{
    std::cout << __PRETTY_FUNCTION__ << ": " << (char*)data << std::endl;
    std::cout << "data[0] = " << std::to_string(data[0]) << std::endl;
    if (data[0] == 65)
    {
        exit(0);
    }
    return SensorMessage(0);
}

void DeltaT100Controller::requestSensorPing(const SwitchDataCommand& command)
{
    sensor_communication_.sendRequest(
        dynamic_cast<const SensorMessage&>(command), SonarReturnData::command_length_, false);
}

DeltaT100Controller::~DeltaT100Controller()
{
    // TODO Auto-generated destructor stub
}

} /* namespace Controller */
