/*
 * DeltaT100ControllerInterface.cpp
 *
 *  Created on: 3 Oct 2018
 *      Author: Thomas Maters
 *		Email : thomasmaters@hotmail.com (TG.Maters@student.han.nl)
 */

#include "DeltaT100ControllerProxy.hpp"
#include "../../Communication/IOHandler.hpp"
#include "../../Messages/SonarReturnDataPacket.hpp"

namespace Controller::DeltaT100
{

DeltaT100ControllerProxy::DeltaT100ControllerProxy(
		const std::string& host,
        const std::string& remote_port,
        const std::string& local_port):
		sensor_communication_(Communication::IOHandler::getInstance().getIOService(), host, remote_port, local_port),
	    deltat_communication_(Communication::IOHandler::getInstance().getIOService(), "localhost", remote_port, local_port)
{
    deltat_communication_.addRequestHandler(std::shared_ptr<RequestHandler>(this));
    sensor_communication_.addResponseHandler(std::shared_ptr<ResponseHandler>(this));
}

DeltaT100ControllerProxy::~DeltaT100ControllerProxy()
{
}

void Controller::DeltaT100::DeltaT100ControllerProxy::handleResponse(
		uint8_t*, std::size_t, std::chrono::milliseconds::rep)
{
}

Messages::SensorMessage DeltaT100ControllerProxy::handleRequest(
		uint8_t*, std::size_t, std::chrono::milliseconds::rep)
{
	return Messages::SensorMessage(0);
}

void DeltaT100ControllerProxy::requestSensorPing(const Messages::SwitchDataCommand& command)
{
    sensor_communication_.sendRequest(
        dynamic_cast<const Messages::SensorMessage&>(command), Messages::SonarReturnDataPacket::command_length_, false);
}

}
/* namespace Controller::DeltaT100 */

