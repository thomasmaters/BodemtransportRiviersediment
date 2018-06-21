/*
 * SensorProxyInterface.hpp
 *
 *  Created on: 13 Jun 2018
 *      Author: Thomas Maters
 *		Email : thomasmaters@hotmail.com (TG.Maters@student.han.nl)
 */

#ifndef SRC_CONNECTIONINTERFACE_HPP_
#define SRC_CONNECTIONINTERFACE_HPP_

#include <cstdint>
#include <iostream>

#include "SensorMessage.hpp"
#include "boost/asio.hpp"

namespace Communication
{

//Handles incomming request from to sensor.
class RequestHandler
{
public:
	RequestHandler(){}

	virtual SensorMessage handleRequest(uint8_t* data, std::size_t length) = 0;

	virtual ~RequestHandler()
	{

	}
};

class ResponseHandler
{
public:
	ResponseHandler(){}

	virtual void handleResponse(uint8_t* data, std::size_t length){}

	virtual std::size_t handleResponseHead(uint8_t* data, std::size_t length){ return 10; }

	virtual ~ResponseHandler()
	{

	}
};

class RequestResponseHandler : public RequestHandler, public ResponseHandler
{
};

class ConnectionInterface
{
public:
	virtual ~ConnectionInterface() = default;

	virtual void sendRequest(const SensorMessage& message, std::size_t responseSize, bool hasResponseHeadAndBody = false) = 0;

	virtual void sendRequest(const SensorMessage& message, char delimiter, bool hasResponseHeadAndBody = false) = 0;

	virtual	void addRequestHandler(std::shared_ptr<RequestHandler> aRequestHandler)
	{
        requestHandler.reset();
        requestHandler.swap(aRequestHandler);
	}

	virtual	void addResponseHandler(std::shared_ptr<ResponseHandler> aResponseHandler)
	{
        responseHandler.reset();
        responseHandler.swap(aResponseHandler);
	}
protected:
    std::shared_ptr<RequestHandler> requestHandler;
    std::shared_ptr<ResponseHandler> responseHandler;
};

}




#endif /* SRC_CONNECTIONINTERFACE_HPP_ */
