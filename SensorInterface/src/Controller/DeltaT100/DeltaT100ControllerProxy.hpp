/*
 * DeltaT100ControllerProxy.hpp
 *
 *  Created on: 3 Oct 2018
 *      Author: Thomas Maters
 *		Email : thomasmaters@hotmail.com (TG.Maters@student.han.nl)
 */

#ifndef SRC_CONTROLLER_DELTAT100_DELTAT100CONTROLLERPROXY_HPP_
#define SRC_CONTROLLER_DELTAT100_DELTAT100CONTROLLERPROXY_HPP_

#include "../../Communication/TCP/TCPConnection.hpp"
#include "../../Communication/UDP/UDPConnection.hpp"
#include "../../Messages/SensorMessage.hpp"
#include "../../Messages/SwitchDataCommand.hpp"

namespace Controller::DeltaT100
{

/*
 *
 */
class DeltaT100ControllerProxy: public Communication::RequestResponseHandler
{
public:
	DeltaT100ControllerProxy(const std::string& host,
            const std::string& remote_port,
            const std::string& local_port);

    /**
     * Handles a response from the DeltaT100 Multibeam.
     * @param data
     * @param length of response.
     * @param time of request.
     */
    void handleResponse(uint8_t* data, std::size_t length, std::chrono::milliseconds::rep time = 0) override;

    /**
     * Handles a UDP message from DeltaT.exe.
     * @param data
     * @param length of request
     * @param time of request.
     * @return Response message
     */
    Messages::SensorMessage handleRequest(uint8_t* data, std::size_t length, std::chrono::milliseconds::rep time = 0) override;

	virtual ~DeltaT100ControllerProxy();
protected:
    /**
     * Sends a request for a SonarReturnDataPacket.
     * @param command Switch command.
     */
    void requestSensorPing(const Messages::SwitchDataCommand& command);

private:

    Communication::TCP::TCPServerClient sensor_communication_;
    Communication::UDP::UDPServerClient deltat_communication_;
};

} /* namespace Controller::DeltaT100 */

#endif /* SRC_CONTROLLER_DELTAT100_DELTAT100CONTROLLERPROXY_HPP_ */
