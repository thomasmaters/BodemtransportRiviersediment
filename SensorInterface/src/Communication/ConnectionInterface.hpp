/*
 * SensorProxyInterface.hpp
 *
 *  Created on: 13 Jun 2018
 *      Author: Thomas Maters
 *		Email : thomasmaters@hotmail.com (TG.Maters@student.han.nl)
 */

#ifndef SRC_CONNECTIONINTERFACE_HPP_
#define SRC_CONNECTIONINTERFACE_HPP_

//#define ENABLE_IO_DEBUG 0

#include "../Messages/SensorMessage.hpp"
#include "RequestResponseHandler.hpp"

#include <memory>
#ifdef ENABLE_IO_DEBUG
#include <iostream>
#endif
namespace Communication
{
using namespace Messages;

/**
 * Interface for communicating over different protocols.
 */
class ConnectionInterface
{
  public:
	ConnectionInterface()
	{
#ifdef ENABLE_IO_DEBUG
			std::cout << "ConnectionInterface -> Constructor" << std::endl;
#endif
	}

    virtual ~ConnectionInterface()
    {
#ifdef ENABLE_IO_DEBUG
			std::cout << "ConnectionInterface -> Destructor" << std::endl;
#endif
    	if(request_handler_.use_count() > 0)
    	{
#ifdef ENABLE_IO_DEBUG
			std::cout << "ConnectionInterface -> Requesthandler count: " << request_handler_.use_count() << std::endl;
#endif
			request_handler_.reset();
    	}
    	if(response_handler_.use_count() > 0)
    	{
#ifdef ENABLE_IO_DEBUG
			std::cout << "ConnectionInterface -> Responsehandler count: " << response_handler_.use_count() << std::endl;
#endif
			response_handler_.reset();
    	}
    }

    /**
     * Sends a message.
     * @param message Message to send.
     * @param response_size Amount of bytes of response.
     * @param has_response_head_and_body Read response in 2 parts.
     */
    virtual void sendRequest(const SensorMessage& message,
                             std::size_t response_size,
                             bool has_response_head_and_body = false) = 0;

    /**
     * Sends a message.
     * @param message Message to send.
     * @param delimiter Read until byte.
     * @param has_response_head_and_body Read response in 2 parts.
     */
    virtual void sendRequest(const SensorMessage& message, char delimiter, bool has_response_head_and_body = false) = 0;

    /**
     * Adds callback when a request comes in.
     * @param request_handler
     */
    virtual void addRequestHandler(std::shared_ptr<RequestHandler> request_handler)
    {
        request_handler_.reset();
        request_handler_.swap(request_handler);
    }

    /**
     * Adds callback for received responses.
     * @param response_handler
     */
    virtual void addResponseHandler(std::shared_ptr<ResponseHandler> response_handler)
    {
        response_handler_.reset();
        response_handler_.swap(response_handler);
    }

    /**
     * Get the current time of the system in epoch milliseconds.
     * @return Epoch time in milliseconds
     */
    static std::chrono::milliseconds::rep getCurrentTime()
    {
        return std::chrono::duration_cast<std::chrono::milliseconds>(
                   std::chrono::system_clock::now().time_since_epoch())
            .count();
    }

  protected:
    std::shared_ptr<RequestHandler> request_handler_;
    std::shared_ptr<ResponseHandler> response_handler_;
};
}

#endif /* SRC_CONNECTIONINTERFACE_HPP_ */
//Verantwoording van de kwaliteit
//Waarom mijn oplossings is van het probleem, validiteit aantonen van producten en resultaten
//Makkelijkere beschirjven opdracht.(overview)
//Overal conclusie in stageverslag
//Berekenen sediment, linken aan componenten.
//20min presentatie.
//min demo
//Link diagrammen naar wat ze doen aka overview.
