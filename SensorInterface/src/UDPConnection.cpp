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

#include <iostream>

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
    std::cout << __PRETTY_FUNCTION__ << " local: " << local_port << " remote: " << remote_port << std::endl;
    start_receive();
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
    std::cout << __PRETTY_FUNCTION__ << std::endl;
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
        std::cout << ec.message() << std::endl;
    }
    return !ec;
}

void UDPServerClient::sendMessage(const boost::asio::mutable_buffer& buffer,
                                  std::size_t response_size,
                                  bool has_response_head_and_body)
{
    if (connectOutgoingSocket())
    {
        std::cout << __PRETTY_FUNCTION__ << std::endl;
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
                                  std::size_t bytes_transferred)
{
    std::cout << "WRITTEN: " << bytes_transferred << " bytes, trying to receive: " << response_size << " bytes."
              << std::endl;
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
        std::cerr << __PRETTY_FUNCTION__ << ": " << error.message() << std::endl;
    }
}

void UDPServerClient::gotResponse([[maybe_unused]] bool has_response_head_and_body,
                                  const boost::system::error_code& error,
                                  std::size_t bytes_transferred)
{
    std::cout << __PRETTY_FUNCTION__ << ": bytes transferd: " << bytes_transferred << std::endl;
    if (error)
    {
        std::cout << __PRETTY_FUNCTION__ << ": " << error.message() << std::endl;
        // throw std::runtime_error("Failed getting response." + error.message());
        return;
    }

    if (response_handler_.use_count() != 0)
    {
        response_handler_->handleResponse(data_.data(), bytes_transferred);
    }
}

void UDPServerClient::start_receive()
{
    std::cout << __PRETTY_FUNCTION__ << std::endl;
    socket_incomming_.async_receive_from(boost::asio::buffer(data_, UDP_BUFFER_SIZE),
                                         local_endpoint_,
                                         boost::bind(&UDPServerClient::handle_receive,
                                                     this,
                                                     boost::asio::placeholders::error,
                                                     boost::asio::placeholders::bytes_transferred));
}

void UDPServerClient::handle_receive(const boost::system::error_code& error, std::size_t bytes_transferred)
{
    start_receive();
    std::cout << "Received: " << bytes_transferred << " bytes" << std::endl;
    if (!error || (error == boost::asio::error::eof && bytes_transferred > 0))
    {
        if (request_handler_.use_count() != 0)
        {
            SensorMessage response = request_handler_->handleRequest(data_.data(), bytes_transferred);
            if (response.getDataLength() > 0)
            {
                socket_incomming_.async_send_to(boost::asio::buffer(response.getData(), response.getDataLength()),
                                                local_endpoint_,
                                                boost::bind(&UDPServerClient::handle_send,
                                                            this,
                                                            boost::asio::placeholders::error,
                                                            boost::asio::placeholders::bytes_transferred));
            }
            else
            {
                std::cout << "Response to message is empty, stopping contact with client" << std::endl;
            }
        }
    }
    else
    {
        std::cout << "Error occurred: " << error.message() << std::endl;
    }
}

void UDPServerClient::handle_send([[maybe_unused]] const boost::system::error_code& error,
                                  std::size_t bytes_transferred)
{
    std::cout << "Send response of: " << bytes_transferred << " bytes" << std::endl;
}
}
