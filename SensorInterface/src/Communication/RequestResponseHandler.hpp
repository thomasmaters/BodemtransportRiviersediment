/*
 * RequestResponseHandler.hpp
 *
 *  Created on: 24 Sep 2018
 *      Author: Thomas Maters
 *		Email : thomasmaters@hotmail.com (TG.Maters@student.han.nl)
 */

#ifndef SRC_COMMUNICATION_REQUESTRESPONSEHANDLER_HPP_
#define SRC_COMMUNICATION_REQUESTRESPONSEHANDLER_HPP_

#include "../Messages/SensorMessage.hpp"

namespace Communication
{
	using namespace Messages;

/**
 * Class that enables the derived class of receiving requests.
 */
class RequestHandler
{
  public:
    RequestHandler()
    {
    }

    /**
     * Callback for when a request comes in.
     * @param data
     * @param length
     * @param time
     * @return
     */
    virtual SensorMessage handleRequest(uint8_t* data, std::size_t length, std::chrono::milliseconds::rep time = 0) = 0;

    virtual ~RequestHandler()
    {
    }
};

/**
 * Class that enables the derived class of receiving responses on requests.
 */
class ResponseHandler
{
  public:
    ResponseHandler()
    {
    }

    /**
     * Callback when a response on a request comes in.
     * @param data
     * @param length
     * @param time
     */
    virtual void handleResponse([[maybe_unused]] uint8_t* data,
                                [[maybe_unused]] std::size_t length,
                                [[maybe_unused]] std::chrono::milliseconds::rep time = 0)
    {
    }

    /**
     * Callback when a response on a request comes in with a expected request header.
     * @param data
     * @param length
     * @return
     */
    virtual std::size_t handleResponseHead([[maybe_unused]] uint8_t* data, [[maybe_unused]] std::size_t length)
    {
        return 0;
    }

    virtual ~ResponseHandler()
    {
    }
};

/**
 * Convenience class.
 */
class RequestResponseHandler : public RequestHandler, public ResponseHandler
{
};
}// Namespace Communication


#endif /* SRC_COMMUNICATION_REQUESTRESPONSEHANDLER_HPP_ */
