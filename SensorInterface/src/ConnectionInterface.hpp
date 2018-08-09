/*
 * SensorProxyInterface.hpp
 *
 *  Created on: 13 Jun 2018
 *      Author: Thomas Maters
 *		Email : thomasmaters@hotmail.com (TG.Maters@student.han.nl)
 */

#ifndef SRC_CONNECTIONINTERFACE_HPP_
#define SRC_CONNECTIONINTERFACE_HPP_

#define ENABLE_IO_DEBUG 1

#include "SensorMessage.hpp"

#include <cstdint>
#include <memory>

namespace Communication
{
// Handles incomming request from to sensor.
class RequestHandler
{
  public:
    RequestHandler()
    {
    }

    virtual SensorMessage handleRequest(uint8_t* data, std::size_t length, std::chrono::milliseconds::rep time = 0) = 0;

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

    virtual void handleResponse([[maybe_unused]] uint8_t* data, [[maybe_unused]] std::size_t length, [[maybe_unused]] std::chrono::milliseconds::rep time = 0)
    {
    }

    virtual std::size_t handleResponseHead([[maybe_unused]] uint8_t* data, [[maybe_unused]] std::size_t length)
    {
        return 10;
    }

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

    virtual void sendRequest(const SensorMessage& message,
                             std::size_t response_size,
                             bool has_response_head_and_body = false) = 0;

    virtual void sendRequest(const SensorMessage& message, char delimiter, bool has_response_head_and_body = false) = 0;

    virtual void addRequestHandler(std::shared_ptr<RequestHandler> request_handler)
    {
        request_handler_.reset();
        request_handler_.swap(request_handler);
    }

    virtual void addResponseHandler(std::shared_ptr<ResponseHandler> response_handler)
    {
        response_handler_.reset();
        response_handler_.swap(response_handler);
    }

    static std::chrono::milliseconds::rep getCurrentTime()
    {
    	return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    }

  protected:
    std::shared_ptr<RequestHandler> request_handler_;
    std::shared_ptr<ResponseHandler> response_handler_;
};
}

#endif /* SRC_CONNECTIONINTERFACE_HPP_ */
