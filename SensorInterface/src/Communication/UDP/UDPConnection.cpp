/*
 * UDPConnection.cpp
 *
 *  Created on: 22 Jun 2018
 *      Author: Thomas Maters
 *		Email : thomasmaters@hotmail.com (TG.Maters@student.han.nl)
 */

#include "UDPConnection.hpp"

#include <boost/asio/connect.hpp>
#include <boost/asio/placeholders.hpp>
#include <boost/bind.hpp>

#ifdef ENABLE_IO_DEBUG
#include <iostream>
#endif

namespace Communication::UDP
{
UDPServerClient::UDPServerClient(boost::asio::io_service& io_service,
                                 const std::string& host,
                                 const std::string& remote_port,
                                 const std::string& local_port)
  : host_(host),
    remote_port_(remote_port),
    local_port_(local_port),
    io_service_(io_service),
    socket_outgoing_(io_service),
    socket_incomming_(io_service, udp::endpoint(udp::v4(), std::atoi(local_port.c_str())))
{
#ifdef ENABLE_IO_DEBUG
			std::cout << "UDPServerClient -> Constructor" << std::endl;
#endif
    startReceive();
}

void UDPServerClient::sendRequest(const SensorMessage& message,
                                  std::size_t response_size,
                                  bool has_response_head_and_body)
{
    if (has_response_head_and_body)
    {
        throw std::runtime_error("UDP connection does not support reading parts of a datagram.");
    }
    sendMessage(boost::asio::mutable_buffer(message.getData(), message.getDataLength()),
                response_size,
                has_response_head_and_body);
}

void UDPServerClient::sendRequest([[maybe_unused]] const SensorMessage& message,
                                  [[maybe_unused]] char delimiter,
                                  [[maybe_unused]] bool has_response_head_and_body)
{
    throw std::runtime_error("UDP connection does not support reading until a delimiter, use a response size instead.");
}

void UDPServerClient::sendRequest(std::string message, std::size_t response_size)
{
    sendMessage(boost::asio::mutable_buffer(message.data(), message.length()), response_size, false);
}

UDPServerClient::~UDPServerClient()
{
#ifdef ENABLE_IO_DEBUG
			std::cout << "UDPServerClient -> Destructor" << std::endl;
#endif
	if (socket_incomming_.is_open()) {
		socket_incomming_.shutdown(boost::asio::socket_base::shutdown_type::shutdown_both);
		socket_incomming_.cancel();
		socket_incomming_.close();
	}
	if( socket_outgoing_.is_open())
	{
		socket_outgoing_.shutdown(boost::asio::socket_base::shutdown_type::shutdown_both);
		socket_outgoing_.cancel();
		socket_outgoing_.close();
	}
}

bool UDPServerClient::connectOutgoingSocket()
{
    boost::asio::ip::udp::resolver resolver(io_service_);
    boost::asio::ip::udp::resolver::query query(boost::asio::ip::udp::v4(), host_, remote_port_);
    boost::asio::ip::udp::resolver::iterator iter = resolver.resolve(query);
    boost::system::error_code ec;
    boost::asio::connect(socket_outgoing_, iter, ec);
    if (ec)
    {
#ifdef ENABLE_IO_DEBUG
        std::cout << "UDPServerClient -> Failed to connect to outgoing socket: " << ec.message() << std::endl;
#endif
    }
    return !ec;
}

void UDPServerClient::sendMessage(const boost::asio::mutable_buffer& buffer,
                                  std::size_t response_size,
                                  bool has_response_head_and_body)
{
    if (connectOutgoingSocket())
    {
#ifdef ENABLE_IO_DEBUG
        std::cout << "UDPServerClient -> SENDING( " << buffer.size() << ")" << std::endl;
#endif
        socket_outgoing_.async_send(buffer,
                                    boost::bind(&UDPServerClient::getResponse,
                                                this,
                                                response_size,
                                                has_response_head_and_body,
                                                boost::asio::placeholders::error,
                                                boost::asio::placeholders::bytes_transferred));
    }
}

void UDPServerClient::getResponse(std::size_t response_size,
                                  bool has_response_head_and_body,
                                  const boost::system::error_code& error,
								  [[maybe_unused]] std::size_t bytes_transferred)
{
#ifdef ENABLE_IO_DEBUG
    std::cout << "UDPServerClient -> SEND( " << bytes_transferred << ")" << std::endl;
#endif
    if (!error)
    {
        if (response_size > 0)
        {
            socket_outgoing_.async_receive_from(boost::asio::buffer(data_, response_size),
                                                remote_endpoint_,
                                                boost::bind(&UDPServerClient::gotResponse,
                                                            this,
                                                            has_response_head_and_body,
                                                            boost::asio::placeholders::error,
                                                            boost::asio::placeholders::bytes_transferred));
        }
    }
    else
    {
#ifdef ENABLE_IO_DEBUG
        std::cout << "UDPServerClient -> Send failed: " << error.message() << std::endl;
#endif
    }
}

void UDPServerClient::gotResponse([[maybe_unused]] bool has_response_head_and_body,
                                  const boost::system::error_code& error,
                                  std::size_t bytes_transferred)
{
#ifdef ENABLE_IO_DEBUG
    std::cout << "UDPServerClient -> GOT RESPONSE( " << bytes_transferred << ")" << std::endl;
#endif
    if (!error)
    {
        if (response_handler_.use_count() != 0)
        {
            response_handler_->handleResponse(data_.data(), bytes_transferred, ConnectionInterface::getCurrentTime());
        }
        return;
    }
    else
    {
#ifdef ENABLE_IO_DEBUG
        std::cout << "UDPServerClient -> Failed to get response: " << error.message() << std::endl;
#endif
    }
}

void UDPServerClient::startReceive()
{
    socket_incomming_.async_receive_from(boost::asio::buffer(data_, UDP_BUFFER_SIZE),
                                         local_endpoint_,
                                         boost::bind(&UDPServerClient::handleReceive,
                                                     this,
                                                     boost::asio::placeholders::error,
                                                     boost::asio::placeholders::bytes_transferred));
}

void UDPServerClient::handleReceive(const boost::system::error_code& error, std::size_t bytes_transferred)
{
#ifdef ENABLE_IO_DEBUG
    std::cout << "UDPServerClient -> RECEIVED REQUEST( " << bytes_transferred << ")" << std::endl;
#endif
    startReceive();
    if (!error || (error == boost::asio::error::eof && bytes_transferred > 0))
    {
        if (request_handler_.use_count() != 0)
        {
            SensorMessage response =
                request_handler_->handleRequest(data_.data(), bytes_transferred, ConnectionInterface::getCurrentTime());
            if (response.getDataLength() > 0)
            {
#ifdef ENABLE_IO_DEBUG
                std::cout << "UDPServerClient -> SENDING RESPONSE( " << response.getDataLength() << ")" << std::endl;
#endif
                socket_incomming_.async_send_to(boost::asio::buffer(response.getData(), response.getDataLength()),
                                                local_endpoint_,
                                                boost::bind(&UDPServerClient::handleSend,
                                                            this,
                                                            boost::asio::placeholders::error,
                                                            boost::asio::placeholders::bytes_transferred));
            }
            else
            {
#ifdef ENABLE_IO_DEBUG
                std::cout << "UDPServerClient -> Response to request is empty, closing connection" << std::endl;
#endif
            }
        }
    }
    else
    {
#ifdef ENABLE_IO_DEBUG
        std::cout << "UDPServerClient -> Receive failed: " << error.message() << std::endl;
#endif
    }
}

void UDPServerClient::handleSend([[maybe_unused]]const boost::system::error_code& error, [[maybe_unused]] std::size_t bytes_transferred)
{
#ifdef ENABLE_IO_DEBUG
    std::cout << "UDPServerClient -> SEND( " << bytes_transferred << ")" << std::endl;
#endif
}
}
