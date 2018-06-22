/*
 * DeltaT100Controller.cpp
 *
 *  Created on: 13 Jun 2018
 *      Author: Thomas Maters
 *		Email : thomasmaters@hotmail.com (TG.Maters@student.han.nl)
 */

#include "DeltaT100Controller.hpp"

#include "SwitchDataCommand.hpp"

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
  : io_service_(io_service), sensor_communication_(io_service_, host, std::atoi(local_port.c_str()))
{
    std::cout << __PRETTY_FUNCTION__ << std::endl;
    sensor_communication_.addRequestHandler(std::shared_ptr<RequestHandler>(this));
    sensor_communication_.addResponseHandler(std::shared_ptr<ResponseHandler>(this));
    std::random_device rd;
    std::mt19937 generator(rd());

    std::thread a(std::thread([&] {
        SensorMessage temp = Controller::DeltaT100::SwitchDataCommand::getDefaultInstance();
        sensor_communication_.sendRequest(temp, (std::size_t)5, true);
        std::this_thread::sleep_for(std::chrono::seconds(5));
        while (1)
        {
            if (true)
            {
                std::string str("0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz");

                std::shuffle(str.begin(), str.end(), generator);

                sensor_communication_.sendRequest(str.substr(0, 5), 10);
            }
            std::this_thread::sleep_for(std::chrono::seconds(15));
        }
    }));
    auto work = std::make_shared<boost::asio::io_service::work>(io_service_);
    io_service_.run();
    std::cout << "Preemptive exit" << std::endl;
    a.join();
}

void DeltaT100Controller::handleResponse(uint8_t* data, std::size_t length)
{
    std::cout << __PRETTY_FUNCTION__ << ": " << (char*)data << std::endl;
}

SensorMessage DeltaT100Controller::handleRequest(uint8_t* data, std::size_t length)
{
    std::cout << __PRETTY_FUNCTION__ << ": " << (char*)data << std::endl;
    return SensorMessage(0);
}

DeltaT100Controller::~DeltaT100Controller()
{
    // TODO Auto-generated destructor stub
}

} /* namespace Controller */
