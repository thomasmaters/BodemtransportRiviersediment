/*
 * SensorProxyInterface.hpp
 *
 *  Created on: 13 Jun 2018
 *      Author: Thomas Maters
 *		Email : thomasmaters@hotmail.com (TG.Maters@student.han.nl)
 */

#ifndef SRC_SERVERINTERFACE_HPP_
#define SRC_SERVERINTERFACE_HPP_

#include <cstdint>
#include <iostream>

#include "SensorMessage.hpp"
#include "boost/asio.hpp"

class ServerInterface
{
public:
	ServerInterface()
	{

	}

	virtual ~ServerInterface()
	{

	}

	virtual void closeConnection() = 0;

	virtual bool isConnectionOpen() = 0;

	virtual std::size_t sendRequest(uint8_t* data, std::size_t length);

	virtual std::size_t sendRequest(const SensorMessage& message) = 0;
private:
//	std::shared_ptr<> TODO hadler for request
};

class IoServiceHandler
{
public:
	 static IoServiceHandler& getCommunicationService()
		{
			static IoServiceHandler communicationService;
			return communicationService;
		}
		/**
		 *
		 */
		boost::asio::io_service& getIOService()
		{
			return io_service;
		}
private:
	IoServiceHandler()
	{
		std::cout << __PRETTY_FUNCTION__ << std::endl;
		std::thread temp = std::thread([this]{
				std::cout << getIOService().stopped() << std::endl;
				getIOService().run();
				std::cout << getIOService().stopped() << std::endl;
		});

		requestHandlerThread.swap(temp);
	}

	std::thread requestHandlerThread;
	boost::asio::io_service io_service;
};

//Handles incomming request from to sensor.
class RequestHandler
{
public:
	virtual void handleRequest(uint8_t* data, std::size_t length);

	virtual ~RequestHandler()
	{

	}
};

class ResponseHandler
{
public:
	ResponseHandler()
{

}

	//Handles incomming response from last request.
	virtual void handleResponse(uint8_t* data, std::size_t length);

	virtual ~ResponseHandler()
	{

	}
};

class RequestResponseHandler : public RequestHandler, public ResponseHandler
{
};



#endif /* SRC_SERVERINTERFACE_HPP_ */
