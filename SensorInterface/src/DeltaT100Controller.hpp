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
#include "UDPConnection.hpp"

namespace Controller::DeltaT100
{
/*
 *
 */
class DeltaT100Controller : public Communication::RequestHandler, public Communication::ResponseHandler
{
  public:
    //	DeltaT100Controller();

    DeltaT100Controller(boost::asio::io_service& aService, const std::string& host, const std::string& localPort,
                        const std::string& remotePort);

    void handleResponse(uint8_t* data, std::size_t length);

    SensorMessage handleRequest(uint8_t* data, std::size_t length);

    virtual ~DeltaT100Controller();

  private:
    boost::asio::io_service& service;
    Communication::UDP::UDPServerClient com;
};

} /* namespace Controller */

#endif /* SRC_DELTAT100CONTROLLER_HPP_ */
