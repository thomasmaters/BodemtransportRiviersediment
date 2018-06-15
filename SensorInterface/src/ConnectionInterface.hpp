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

class IoServiceHandler
{
public:
	IoServiceHandler()
	{
		std::cout << __PRETTY_FUNCTION__ << std::endl;
		std::thread temp = std::thread([this]{
			getIOService().run();
		});

		requestHandlerThread.swap(temp);
	}

	boost::asio::io_service& getIOService()
	{
		return io_service;
	}

	virtual ~IoServiceHandler() {}

protected:
	std::thread requestHandlerThread;
	boost::asio::io_service io_service;
};

class ConnectionInterface
{
public:
	virtual ~ConnectionInterface() = default;

	virtual void sendRequest(uint8_t* data, std::size_t length) = 0;

	virtual void sendRequest(const SensorMessage& message) = 0;

	virtual void sendRequest(const std::string& message) = 0;
private:
//	std::shared_ptr<> TODO hadler for request
};

//Handles incomming request from to sensor.
class RequestHandler
{
public:
	RequestHandler(){}

	virtual void handleRequest(uint8_t* data, std::size_t length){}

	virtual ~RequestHandler()
	{

	}
};

class ResponseHandler
{
public:
	ResponseHandler(){}

	virtual void handleResponse(uint8_t* data, std::size_t length){}

	virtual ~ResponseHandler()
	{

	}
};

class RequestResponseHandler : public RequestHandler, public ResponseHandler
{
};

}



#endif /* SRC_CONNECTIONINTERFACE_HPP_ */
