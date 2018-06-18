/*
 * DeltaT100Controller.hpp
 *
 *  Created on: 13 Jun 2018
 *      Author: Thomas Maters
 *		Email : thomasmaters@hotmail.com (TG.Maters@student.han.nl)
 */

#ifndef SRC_DELTAT100CONTROLLER_HPP_
#define SRC_DELTAT100CONTROLLER_HPP_

#include "ConnectionInterface.hpp"
#include "TCPConnection.hpp"

namespace Controller::DeltaT
{
/*
 *
 */
class DeltaT100Controller : public Communication::RequestHandler, public Communication::ResponseHandler
{
public:
//	DeltaT100Controller();

	DeltaT100Controller(boost::asio::io_service& aService, const std::string& host, const std::string& localPort, const std::string& remotePort);

	void handleResponse(uint8_t* data, std::size_t length);

	SensorMessage handleRequest(uint8_t* data, std::size_t length);

	virtual ~DeltaT100Controller();
private:
	 boost::asio::io_service& service;
	 Communication::TCP::TCPServerClient com;
};

} /* namespace Controller */

#endif /* SRC_DELTAT100CONTROLLER_HPP_ */
