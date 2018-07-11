/*
 * TCPConnection.cpp
 *
 *  Created on: 22 Jun 2018
 *      Author: Thomas Maters
 *		Email : thomasmaters@hotmail.com (TG.Maters@student.han.nl)
 */

#include "TCPConnection.hpp"

#include <boost/asio/placeholders.hpp>
#include <boost/asio/read.hpp>
#include <boost/asio/read_until.hpp>
#include <boost/asio/streambuf.hpp>
#include <boost/asio/write.hpp>
#include <boost/bind.hpp>

#include <iostream>

namespace Communication::TCP
{
TCPSession::TCPSession(boost::asio::io_service& io_service, bool keep_alive)
  : socket_(io_service), keep_alive_(keep_alive)
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
    std::cout << "SEND( " << message.size() << ")" << std::endl;
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
    std::cout << "Send: " << bytes_transferd << " bytes." << std::endl;
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
        std::cout << "FAILED TO GET RESPONSE: " << error.message() << std::endl;
    }
}

void TCPSession::handleReceivedResponse(bool has_response_head_and_body,
                                        const boost::system::error_code& error,
                                        std::size_t bytes_transferd,
                                        std::shared_ptr<ResponseHandler> response_handler)
{
    std::cout << "RESPONSE(" << bytes_transferd << "): " << std::endl;
    if (response_handler.use_count() > 0)
    {
        if (has_response_head_and_body)
        {
            std::size_t bodySize = response_handler->handleResponseHead(data_.data(), bytes_transferd);
            handleResponse(bodySize, false, boost::system::error_code(), 0, response_handler);
        }
        else
        {
            response_handler->handleResponse(data_.data(), bytes_transferd);
        }
    }
}

void TCPSession::handleRequest(const boost::system::error_code& error,
                               std::size_t bytes_transferd,
                               std::shared_ptr<RequestHandler> request_handler)
{
    if (!error || error == boost::asio::error::eof)
    {
        std::cout << "We got request with data: " << data_.data() << std::endl;
        if (request_handler.use_count() > 0)
        {
            request_handler->handleRequest(data_.data(), bytes_transferd);
        }
        // TODO; request response size.
        boost::asio::async_write(getSocket(),
                                 boost::asio::buffer(data_.data(), 5),
                                 [&](const boost::system::error_code& error, std::size_t byteWritten) {
                                     std::cout << "Written response(" << byteWritten << ")" << std::endl;
                                 });
    }
    else
    {
        std::cout << "INVALID REQUEST RECEIVED: " << error.message() << std::endl;
    }
}

//---------------------------------------------------------
//----------------TCPServerClient--------------------------
//---------------------------------------------------------

TCPServerClient::TCPServerClient(boost::asio::io_service& io_service,
                                 std::string host,
                                 std::string remote_port,
                                 std::string local_port)
  : host_(host),
    remote_port_(remote_port),
    local_port_(local_port),
    acceptor_(io_service, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), std::atoi(local_port_.c_str()))),
    io_service_(io_service)
{
    start_accept();
}

void TCPServerClient::sendRequest(const SensorMessage& message,
                                  std::size_t response_size,
                                  bool has_response_head_and_body)
{
    sendMessage(boost::asio::mutable_buffer(message.getData(), message.getDataLength()),
                response_size,
                has_response_head_and_body);
}

void TCPServerClient::sendRequest(const SensorMessage& message, char delimiter, bool has_response_head_and_body)
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

void TCPServerClient::start_accept()
{
    std::shared_ptr<TCPSession> session = std::shared_ptr<TCPSession>(new TCPSession(io_service_));
    try
    {
        acceptor_.async_accept(
            session->getSocket(),
            boost::bind(&TCPServerClient::handle_accept, this, session, boost::asio::placeholders::error));
    }
    catch (std::exception& e)
    {
        std::cout << "Could not connect: " << e.what() << std::endl;
    }
}

void TCPServerClient::handle_accept(std::shared_ptr<TCPSession> new_connection, const boost::system::error_code& error)
{
    if (!error)
    {
        new_connection->handleIncommingConnection(request_handler_);
        start_accept();
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
        std::cout << "CONNECTED TO HOST" << std::endl;
        active_sesion_->handleOutgoingConnection(
            message_buffer, response_indentifier, has_response_head_and_body, response_handler_);
    }
    else
    {
        std::cout << "COULD NOT CONNECT TO HOST: " << error.message() << std::endl;
    }
}
}
