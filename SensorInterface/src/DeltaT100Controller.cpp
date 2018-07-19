/*
 * DeltaT100Controller.cpp
 *
 *  Created on: 13 Jun 2018
 *      Author: Thomas Maters
 *		Email : thomasmaters@hotmail.com (TG.Maters@student.han.nl)
 */

#include "DeltaT100Controller.hpp"

#include "DepthProfiler.hpp"
#include "Filter.hpp"
#include "SonarReturnData.hpp"
#include "SonarReturnDataPacket.hpp"

#include <iostream>
#include <string>
#include <thread>

namespace Controller::DeltaT100
{
DeltaT100Controller::DeltaT100Controller(boost::asio::io_service& io_service,
                                         const std::string& host,
                                         const std::string& local_port,
                                         const std::string& remote_port,
										 std::size_t setting_start)
  : io_service_(io_service),
    sensor_communication_(io_service_, host, local_port, remote_port),
    deltat_communication_(io_service_, "localhost", local_port, remote_port),
    data_buffer_(std::unique_ptr<DataBuffer<>>(new DataBuffer<>())),
    display_gain_(20),
    current_display_gain_(0),
	received_pings_(0),
	current_setting_(setting_start)
{
    std::cout << __PRETTY_FUNCTION__ << std::endl;
    deltat_communication_.addRequestHandler(std::shared_ptr<RequestHandler>(this));
    sensor_communication_.addResponseHandler(std::shared_ptr<ResponseHandler>(this));

    auto work = std::make_shared<boost::asio::io_service::work>(io_service_);

    switch_data_command_ = SwitchDataCommand();
    sensor_communication_.sendRequest(switch_data_command_, SonarReturnDataPacket::command_length_, false);
    io_service_.run();
}

void DeltaT100Controller::handleResponse(uint8_t* data, std::size_t length)
{
//    std::cout << __PRETTY_FUNCTION__ << std::endl;
    if (length == SonarReturnDataPacket::command_length_)
    {
        // TODO: should we check if we are in bounds of a SonarReturnData message. length == SonarReturnData::length?
        std::unique_ptr<uint8_t[]>& stored_data = data_buffer_->moveToBuffer(data, length);
        SonarReturnDataPacket sonar_data(&(stored_data.get()[0]));
        if((sonar_data.getSerialStatus() & SerialStatus::SWITCHERROR) == SerialStatus::SWITCHERROR ||
		(sonar_data.getSerialStatus() & SerialStatus::CHARSOVERRUN) == SerialStatus::CHARSOVERRUN ||
		(sonar_data.getSerialStatus() & SerialStatus::PRHERROR) == SerialStatus::PRHERROR)
        {
			switch_data_command_.setPulseLength(kaas[current_setting_][0]);
			switch_data_command_.setFrequency((Controller::DeltaT100::SwitchDataCommand::Frequency)kaas[current_setting_][1]);
			switch_data_command_.setRange((Controller::DeltaT100::Range)kaas[current_setting_][2]);
			switch_data_command_.setDataPoints((Controller::DeltaT100::Mode)kaas[current_setting_][3]);
			switch_data_command_.setStartGain(kaas[current_setting_][4]);
			switch_data_command_.setAbsorption(kaas[current_setting_][5]);
			switch_data_command_.setAgcThreshold(kaas[current_setting_][6]);
			std::cout << std::endl << "---------------TRYING SETTING: " << current_setting_ << "/8192 -----------------" << std::endl;
			current_setting_++;
			received_pings_ = 0;
			switch_data_command_.setPacketNumberRequest(0);
			cosntructSensorPing(sonar_data.getMode());
			return;
        }


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
        std::cout << "Length mismatch expected: " << std::to_string(SonarReturnDataPacket::command_length_)
                  << " got: " << std::to_string(length) << std::endl;
    }
}

void DeltaT100Controller::cosntructSensorPing(Mode mode)
{
	if(received_pings_ > 30)
	{
		switch_data_command_.setPulseLength(kaas[current_setting_][0]);
		switch_data_command_.setFrequency((Controller::DeltaT100::SwitchDataCommand::Frequency)kaas[current_setting_][1]);
		switch_data_command_.setRange((Controller::DeltaT100::Range)kaas[current_setting_][2]);
		switch_data_command_.setDataPoints((Controller::DeltaT100::Mode)kaas[current_setting_][3]);
		switch_data_command_.setStartGain(kaas[current_setting_][4]);
		switch_data_command_.setAbsorption(kaas[current_setting_][5]);
		switch_data_command_.setAgcThreshold(kaas[current_setting_][6]);
		std::cout << std::endl << "---------------TRYING SETTING: " << current_setting_ << "/8192 -----------------" << std::endl;
		current_setting_++;
		received_pings_ = 0;
	}
	received_pings_++;
    // Construct a ping by moving the buffer into the ping.
    SonarReturnData ping(data_buffer_, switch_data_command_);
    // Create a new buffer.
	FileHandler file;
	file.openFile("output.837");
	file.getOutputStream().write((char*)ping.getData(), ping.getDataSize());
//    file.getOutputStream() << ping.getData();
    data_buffer_ = std::unique_ptr<DataBuffer<>>(new DataBuffer<>());

    sensor_communication_.sendRequest(switch_data_command_, SonarReturnDataPacket::command_length_, false);
}

SensorMessage DeltaT100Controller::handleRequest(uint8_t* data, std::size_t length)
{
    std::cout << __PRETTY_FUNCTION__ << ": " << length << std::endl;
    ProfilePointOutput sonar_data(data);
    depth_profiler_.addRawPoint(sonar_data.asMatrix());

    if (current_display_gain_ < display_gain_)
    {
        profile_point_output_ += sonar_data;
        current_display_gain_++;
    }
    else
    {
        auto matrix = profile_point_output_.asMatrix();
        ZeroFilter::applyFilter(matrix);
        depth_profiler_.addProcessedPoint(matrix);
        profile_point_output_ = sonar_data;
        current_display_gain_ = 0;
    }

    // Return a message with 1 byte to reopen a connection.
    return SensorMessage(1);
}

void DeltaT100Controller::requestSensorPing(const SwitchDataCommand& command)
{
    sensor_communication_.sendRequest(
        dynamic_cast<const SensorMessage&>(command), SonarReturnDataPacket::command_length_, false);
}

DeltaT100Controller::~DeltaT100Controller()
{
    // TODO Auto-generated destructor stub
}

} /* namespace Controller */
