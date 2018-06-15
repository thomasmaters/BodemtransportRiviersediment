/*
 * DeltaT100Controller.cpp
 *
 *  Created on: 13 Jun 2018
 *      Author: Thomas Maters
 *		Email : thomasmaters@hotmail.com (TG.Maters@student.han.nl)
 */

#include "DeltaT100Controller.hpp"

namespace Controller::DeltaT
{

//DeltaT100Controller::DeltaT100Controller() : com("0.0.0.0", "1337", "1338")
//{
//	std::cout << __PRETTY_FUNCTION__ << std::endl;
//	com.sendRequest("DELTAT100Controller");
//	// TODO Auto-generated constructor stub
//}

DeltaT100Controller::DeltaT100Controller(const std::string& host, const std::string& localPort, const std::string& remotePort)/* : com(host, localPort, remotePort)*/
{
	std::cout << __PRETTY_FUNCTION__ << std::endl;
//	com.sendRequest("DELTAT100Controller1");
//	std::this_thread::sleep_for(std::chrono::seconds(10));
//	com.sendRequest("DELTAT100Controller2");
//	std::this_thread::sleep_for(std::chrono::seconds(10));
//	com.sendRequest("DELTAT100Controller3");
//	std::this_thread::sleep_for(std::chrono::seconds(10));
//	com.sendRequest("DELTAT100Controller4");
//	std::this_thread::sleep_for(std::chrono::seconds(10));
//	ioService.run();
}


void DeltaT100Controller::handleResponse(uint8_t* data, std::size_t length)
{
}

void DeltaT100Controller::handleRequest(uint8_t* data, std::size_t length)
{
}


DeltaT100Controller::~DeltaT100Controller()
{
	// TODO Auto-generated destructor stub
}

} /* namespace Controller */
