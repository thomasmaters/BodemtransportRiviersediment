/*
 * TCPConnection.cpp
 *
 *  Created on: 22 Jun 2018
 *      Author: Thomas Maters
 *		Email : thomasmaters@hotmail.com (TG.Maters@student.han.nl)
 */

#include "TCPConnection.hpp"

#include <boost/asio/placeholders.hpp>
#include <boost/bind.hpp>

#ifdef ENABLE_IO_DEBUG
#include <iostream>
#endif

namespace Communication::TCP
{
TCPServerClient::TCPServerClient(boost::asio::io_service& io_service,
                                 const std::string& host,
                                 const std::string& remote_port,
                                 const std::string& local_port)
  : host_(host),
    remote_port_(remote_port),
    local_port_(local_port),
    acceptor_(io_service, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), std::atoi(local_port_.c_str()))),
    io_service_(io_service)
{
    startAccept();
}

void TCPServerClient::sendRequest(const SensorMessage& message,
                                  std::size_t response_size,
                                  bool has_response_head_and_body)
{
    sendMessage(boost::asio::mutable_buffer(message.getData(), message.getDataLength()),
                response_size,
                has_response_head_and_body);
}

void TCPServerClient::sendRequest(const SensorMessage& message,
                                  char delimiter,
                                  [[maybe_unused]] bool has_response_head_and_body)
{
    sendMessage(boost::asio::mutable_buffer(message.getData(), message.getDataLength()), delimiter, false);
}

void TCPServerClient::sendRequest(std::string& message, std::size_t response_size)
{
    sendMessage(boost::asio::mutable_buffer(message.data(), message.length()), response_size, false);
}

TCPServerClient::~TCPServerClient()
{
    acceptor_.cancel();
    acceptor_.close();
}

void TCPServerClient::startAccept()
{
    std::shared_ptr<TCPSession> session = std::shared_ptr<TCPSession>(new TCPSession(io_service_));
    try
    {
        acceptor_.async_accept(
            session->getSocket(),
            boost::bind(&TCPServerClient::handleAccept, this, session, boost::asio::placeholders::error));
    }
    catch (std::exception& e)
    {
#ifdef ENABLE_IO_DEBUG
        std::cout << "TCPServerClient -> Could not connect: " << e.what() << std::endl;
#endif
    }
}

void TCPServerClient::handleAccept(std::shared_ptr<TCPSession> new_connection, const boost::system::error_code& error)
{
    if (!error)
    {
        new_connection->handleIncommingConnection(request_handler_);
        startAccept();
    }
}

template <typename Type>
void TCPServerClient::sendMessage(const boost::asio::mutable_buffer& message_buffer,
                                  Type response_indentifier,
                                  bool has_response_head_and_body)
{
    static_assert(std::is_same<Type, char>::value || std::is_same<Type, std::size_t>::value,
                  "Message responsetype has to be char or std::size_t.");
    if (active_sesion_.use_count() == 0 || !active_sesion_->isOpen())
    {
        active_sesion_ = std::shared_ptr<TCPSession>(new TCPSession(io_service_));
    }

    if (!active_sesion_->isOpen())
    {
        boost::asio::ip::tcp::resolver resolver(io_service_);
        boost::asio::ip::tcp::resolver::query query(boost::asio::ip::tcp::v4(), host_, remote_port_);
        boost::asio::ip::tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);
        boost::asio::ip::tcp::endpoint endpoint                    = *endpoint_iterator;

        active_sesion_->getSocket().async_connect(endpoint,
                                                  boost::bind(&TCPServerClient::handleConnect<Type>,
                                                              this,
                                                              message_buffer,
                                                              response_indentifier,
                                                              has_response_head_and_body,
                                                              boost::asio::placeholders::error));
    }
    else
    {
        handleConnect(message_buffer, response_indentifier, has_response_head_and_body, boost::system::error_code());
    }
}

template <typename Type>
void TCPServerClient::handleConnect(const boost::asio::mutable_buffer& message_buffer,
                                    Type response_indentifier,
                                    bool has_response_head_and_body,
                                    const boost::system::error_code& error)
{
    if (!error)
    {
#ifdef ENABLE_IO_DEBUG
        std::cout << "TCPServerClient -> CONNECTED TO HOST" << std::endl;
#endif
        active_sesion_->handleOutgoingConnection(
            message_buffer, response_indentifier, has_response_head_and_body, response_handler_);
    }
    else
    {
#ifdef ENABLE_IO_DEBUG
        std::cout << "TCPServerClient -> Could not connect to host: " << error.message() << std::endl;
#endif
    }
}
}
