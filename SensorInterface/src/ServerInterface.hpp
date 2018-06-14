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

#include "SensorMessage.hpp"

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
