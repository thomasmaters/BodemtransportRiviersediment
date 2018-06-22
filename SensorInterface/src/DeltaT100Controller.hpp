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
#include "SerialConnection.hpp"
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

    DeltaT100Controller(boost::asio::io_service& io_service,
                        const std::string& host,
                        const std::string& local_port,
                        const std::string& remote_port);

    void handleResponse(uint8_t* data, std::size_t length);

    SensorMessage handleRequest(uint8_t* data, std::size_t length);

    virtual ~DeltaT100Controller();

  private:
    boost::asio::io_service& io_service_;
    Communication::Serial::SerialClientServer sensor_communication_;
};

} /* namespace Controller */

#endif /* SRC_DELTAT100CONTROLLER_HPP_ */
