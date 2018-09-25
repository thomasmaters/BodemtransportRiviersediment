/*
 * TCPSession.cpp
 *
 *  Created on: 25 Sep 2018
 *      Author: Thomas Maters
 *		Email : thomasmaters@hotmail.com (TG.Maters@student.han.nl)
 */
#include "TCPSession.hpp"

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
            SensorMessage response =
                request_handler->handleRequest(data_.data(), bytes_transferd, ConnectionInterface::getCurrentTime());

            if (response.getDataLength() > 0)
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
