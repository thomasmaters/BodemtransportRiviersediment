/*
 * DeltaT100Controller.cpp
 *
 *  Created on: 13 Jun 2018
 *      Author: Thomas Maters
 *		Email : thomasmaters@hotmail.com (TG.Maters@student.han.nl)
 */

#include "DeltaT100Controller.hpp"

#include "SwitchDataCommand.hpp"

#include <random>
#include <string>

namespace Controller::DeltaT100
{
DeltaT100Controller::DeltaT100Controller(boost::asio::io_service& aService, const std::string& host,
                                         const std::string& localPort, const std::string& remotePort)
  : service(aService), com(aService, host, localPort, remotePort)
{
    std::cout << __PRETTY_FUNCTION__ << std::endl;
    com.addRequestHandler(std::shared_ptr<RequestHandler>(this));
    com.addResponseHandler(std::shared_ptr<ResponseHandler>(this));
    std::random_device rd;
    std::mt19937 generator(rd());

    std::thread a(std::thread([&] {
        SensorMessage temp = Controller::DeltaT100::SwitchDataCommand::getDefaultInstance();
        com.sendRequest(temp, 10);
        std::this_thread::sleep_for(std::chrono::seconds(5));
        while (1)
        {
            if (true)
            {
                std::string str("0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz");

                std::shuffle(str.begin(), str.end(), generator);

                com.sendRequest(str.substr(0, 5), 10);
            }
            std::this_thread::sleep_for(std::chrono::seconds(2));
        }
    }));
    service.run();
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
