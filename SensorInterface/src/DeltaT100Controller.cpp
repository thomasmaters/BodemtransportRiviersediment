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

DeltaT100Controller::DeltaT100Controller(boost::asio::io_service& aService, const std::string& host, const std::string& localPort, const std::string& remotePort) : service(aService), com(aService, host, localPort, remotePort)
{
	std::cout << __PRETTY_FUNCTION__ << std::endl;
	com.addRequestHandler(std::shared_ptr<RequestHandler>(this));
	com.addResponseHandler(std::shared_ptr<ResponseHandler>(this));
	service.run();
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
