/*
 * SerialConnection.cpp
 *
 *  Created on: 22 Jun 2018
 *      Author: Thomas Maters
 *		Email : thomasmaters@hotmail.com (TG.Maters@student.han.nl)
 */

#include "SerialConnection.hpp"

#include <boost/asio/placeholders.hpp>
#include <boost/asio/read_until.hpp>
#include <boost/asio/streambuf.hpp>
#include <boost/bind.hpp>

#ifdef ENABLE_IO_DEBUG
#include <iostream>
#endif

#define BOOST_ASIO_ENABLE_HANDLER_TRACKING 1

namespace Communication::Serial
{
SerialClientServer::SerialClientServer(boost::asio::io_service& aIoService,
                                       const std::string& portName,
                                       uint32_t baudrate)
  : port_name_(portName), baudrate_(baudrate), serial_port_(aIoService), io_service_(aIoService), timer_(aIoService)
{
    boost::system::error_code ec;
    serial_port_.open(portName, ec);
    if (ec)
    {
#ifdef ENABLE_IO_DEBUG
        std::cout << "SerialClientServer -> port_->open() failed...com_port_name=" << portName
                  << ", e=" << ec.message().c_str() << std::endl;
#endif
        return;
    }

    serial_port_.set_option(boost::asio::serial_port_base::baud_rate(baudrate));
    serial_port_.set_option(boost::asio::serial_port_base::character_size(8));
    serial_port_.set_option(boost::asio::serial_port_base::stop_bits(boost::asio::serial_port_base::stop_bits::one));
    serial_port_.set_option(boost::asio::serial_port_base::parity(boost::asio::serial_port_base::parity::none));
    serial_port_.set_option(
        boost::asio::serial_port_base::flow_control(boost::asio::serial_port_base::flow_control::none));
}

void SerialClientServer::sendRequest(const SensorMessage& message,
                                     std::size_t responseSize,
                                     bool hasResponseHeadAndBody)
{
    sendMessage(
        boost::asio::mutable_buffer(message.getData(), message.getDataLength()), responseSize, hasResponseHeadAndBody);
}

void SerialClientServer::sendRequest(const SensorMessage& message, char delimiter, bool hasResponseHeadAndBody)
{
    sendMessage(
        boost::asio::mutable_buffer(message.getData(), message.getDataLength()), delimiter, hasResponseHeadAndBody);
}

void SerialClientServer::sendRequest(std::string message, std::size_t responseSize)
{
    sendMessage(boost::asio::mutable_buffer(message.data(), message.length()), responseSize, false);
}

SerialClientServer::~SerialClientServer()
{
    stop();
}

template <typename Type>
void SerialClientServer::sendMessage(const boost::asio::mutable_buffer& messageBuffer,
                                     Type responseSize,
                                     bool hasResponseHeadAndBody)
{
    if (!serial_port_.is_open())
    {
#ifdef ENABLE_IO_DEBUG
        std::cout << "SerialClientServer -> Serial port not open!" << std::endl;
#endif
    }
    serial_port_.async_write_some(messageBuffer,
                                  boost::bind(&SerialClientServer::handleWriteComplete<Type>,
                                              this,
                                              responseSize,
                                              hasResponseHeadAndBody,
                                              boost::asio::placeholders::error,
                                              boost::asio::placeholders::bytes_transferred));
}

template <typename Type>
void SerialClientServer::handleWriteComplete(Type responseSize,
                                             bool hasResponseHeadAndBody,
                                             const boost::system::error_code& error,
                                             [[maybe_unused]] std::size_t bytes_transferd)
{
    if (error)
    {
#ifdef ENABLE_IO_DEBUG
        std::cout << "SerialClientServer -> Send failed: " << error.message() << std::endl;
#endif
    }

    timer_.expires_from_now(boost::posix_time::seconds(2));
    timer_.async_wait(boost::bind(&SerialClientServer::timerExpired, this, _1));

    if
        constexpr(std::is_same<Type, char>::value)
        {
            boost::asio::streambuf aBuffer;
            boost::asio::async_read_until(serial_port_,
                                          aBuffer,
                                          responseSize,
                                          boost::bind(&SerialClientServer::handleReceivedResponse,
                                                      this,
                                                      hasResponseHeadAndBody,
                                                      boost::asio::placeholders::error,
                                                      boost::asio::placeholders::bytes_transferred));
        }
    else
    {
        serial_port_.async_read_some(boost::asio::buffer(data_, responseSize),
                                     boost::bind(&SerialClientServer::handleReceivedResponse,
                                                 this,
                                                 hasResponseHeadAndBody,
                                                 boost::asio::placeholders::error,
                                                 boost::asio::placeholders::bytes_transferred));
    }
}

void SerialClientServer::handleReceivedResponse(bool hasResponseHeadAndBody,
                                                const boost::system::error_code& error,
                                                std::size_t bytesTransferd)
{
    boost::system::error_code error_c;
    timer_.cancel(error_c);
    if (error)
    {
        return;
    }
#ifdef ENABLE_IO_DEBUG
    std::cout << "SerialClientServer -> GOT RESPONSE(" << bytesTransferd << ")" << std::endl;
#endif
    if (response_handler_.use_count() > 0)
    {
        if (hasResponseHeadAndBody)
        {
            std::size_t bodySize = response_handler_->handleResponseHead(data_.data(), bytesTransferd);
            handleWriteComplete(bodySize, false, boost::system::error_code(), 0);
        }
        else
        {
            response_handler_->handleResponse(data_.data(), bytesTransferd);
        }
    }
}

void SerialClientServer::timerExpired(const boost::system::error_code& error)
{
    if (error != boost::asio::error::operation_aborted)
    {
#ifdef ENABLE_IO_DEBUG
        std::cout << "SerialClientServer -> Timer expired" << std::endl;
#endif
        stop();
    }
}

void SerialClientServer::stop()
{
    if (serial_port_.is_open())
    {
        serial_port_.cancel();
        serial_port_.close();
    }
    io_service_.stop();
    io_service_.reset();
}
}
