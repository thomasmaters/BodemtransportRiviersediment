/*
 * TCPSession.cpp
 *
 *  Created on: 25 Sep 2018
 *      Author: Thomas Maters
 *		Email : thomasmaters@hotmail.com (TG.Maters@student.han.nl)
 */
#include "TCPSession.hpp"

#include <boost/asio/read.hpp>
#include <boost/asio/read_until.hpp>
#include <boost/asio/streambuf.hpp>
#include <boost/asio/write.hpp>
#include <boost/asio/placeholders.hpp>
#include <boost/bind.hpp>

#ifdef ENABLE_IO_DEBUG
#include <iostream>
#endif

namespace Communication::TCP
{
TCPSession::TCPSession(boost::asio::io_service& io_service, bool keep_alive)
  : socket_(io_service), keep_alive_(keep_alive)
{
}

TCPSession::~TCPSession()
{

}

void TCPSession::handleIncommingConnection(std::shared_ptr<RequestHandler> request_handler)
{
    boost::asio::async_read(getSocket(),
                            boost::asio::buffer(data_, TCP_BUFFER_SIZE),
                            boost::bind(&TCPSession::handleRequest,
                                        shared_from_this(),
                                        boost::asio::placeholders::error,
                                        boost::asio::placeholders::bytes_transferred,
                                        request_handler));
}

template <typename Type>
void TCPSession::handleOutgoingConnection(const boost::asio::mutable_buffer& message,
                                          Type response_indentifier,
                                          bool has_response_head_and_body,
                                          std::shared_ptr<ResponseHandler> response_handler)
{
#ifdef ENABLE_IO_DEBUG
    std::cout << "TCPSession -> SENDING( " << message.size() << ")" << std::endl;
#endif
    boost::asio::async_write(getSocket(),
                             boost::asio::buffer(message),
                             boost::bind(&TCPSession::handleResponse<Type>,
                                         shared_from_this(),
                                         response_indentifier,
                                         has_response_head_and_body,
                                         boost::asio::placeholders::error,
                                         boost::asio::placeholders::bytes_transferred,
                                         response_handler));
}

template <typename Type>
void TCPSession::handleResponse(Type response_indentifier,
                                bool has_response_head_and_body,
                                const boost::system::error_code& error,
                                std::size_t bytes_transferd,
                                std::shared_ptr<ResponseHandler> response_handler)
{
#ifdef ENABLE_IO_DEBUG
    std::cout << "TCPSession -> SEND( " << bytes_transferd << ")" << std::endl;
#endif
    if (!error)
    {
        boost::asio::streambuf aBuffer;
        if
            constexpr(std::is_same<Type, char>::value)
            {
                boost::asio::async_read_until(getSocket(),
                                              aBuffer,
                                              response_indentifier,
                                              boost::bind(&TCPSession::handleReceivedResponse,
                                                          shared_from_this(),
                                                          has_response_head_and_body,
                                                          boost::asio::placeholders::error,
                                                          boost::asio::placeholders::bytes_transferred,
                                                          response_handler));
            }
        else
        {
            boost::asio::async_read(getSocket(),
                                    boost::asio::buffer(data_, response_indentifier),
                                    boost::bind(&TCPSession::handleReceivedResponse,
                                                shared_from_this(),
                                                has_response_head_and_body,
                                                boost::asio::placeholders::error,
                                                boost::asio::placeholders::bytes_transferred,
                                                response_handler));
        }
    }
    else
    {
#ifdef ENABLE_IO_DEBUG
        std::cout << "TCPSession -> Failed to get response: " << error.message() << std::endl;
#endif
    }
}

void TCPSession::handleReceivedResponse(bool has_response_head_and_body,
                                        [[maybe_unused]] const boost::system::error_code& error,
                                        std::size_t bytes_transferd,
                                        std::shared_ptr<ResponseHandler> response_handler)
{
#ifdef ENABLE_IO_DEBUG
    std::cout << "TCPSession -> GOT RESPONSE( " << bytes_transferd << ")" << std::endl;
#endif
    if (response_handler.use_count() > 0)
    {
        if (has_response_head_and_body)
        {
            std::size_t bodySize = response_handler->handleResponseHead(data_.data(), bytes_transferd);
            handleResponse(bodySize, false, boost::system::error_code(), 0, response_handler);
        }
        else
        {
            response_handler->handleResponse(data_.data(), bytes_transferd, ConnectionInterface::getCurrentTime());
        }
    }
}

void TCPSession::handleRequest(const boost::system::error_code& error,
                               std::size_t bytes_transferd,
                               std::shared_ptr<RequestHandler> request_handler)
{
#ifdef ENABLE_IO_DEBUG
    std::cout << "TCPSession -> GOT REQUEST( " << bytes_transferd << ")" << std::endl;
#endif
    if (!error || error == boost::asio::error::eof)
    {
        if (request_handler.use_count() > 0)
        {
            SensorMessage response = request_handler->handleRequest(data_.data(), bytes_transferd, ConnectionInterface::getCurrentTime());

            if(response.getDataLength() > 0)
            {
            	boost::asio::async_write(
            	            getSocket(),
            	            boost::asio::buffer(response.getData(), response.getDataLength()),
            	            [&]([[maybe_unused]] const boost::system::error_code& error, std::size_t bytes_written) {
#ifdef ENABLE_IO_DEBUG
	std::cout << "TCPSession -> WRITTEN RESPONSE( " << bytes_written << ")" << std::endl;
#endif
            	            });
            }
            else
            {
#ifdef ENABLE_IO_DEBUG
	std::cout << "TCPSession -> Response to request is empty, closing connection" << std::endl;
#endif
            }
        }

    }
    else
    {
#ifdef ENABLE_IO_DEBUG
        std::cout << "TCPSession -> Invalid request received " << error.message() << std::endl;
#endif
    }
}
}


