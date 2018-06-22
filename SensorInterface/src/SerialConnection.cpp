/*
 * SerialConnection.cpp
 *
 *  Created on: 22 Jun 2018
 *      Author: Thomas Maters
 *		Email : thomasmaters@hotmail.com (TG.Maters@student.han.nl)
 */

#include "SerialConnection.hpp"

#include <boost/bind.hpp>
#include <boost/asio/placeholders.hpp>
#include <boost/asio/streambuf.hpp>
#include <boost/asio/read_until.hpp>

#include <iostream>

//TODO Debugging flag voor async communication
#define BOOST_ASIO_ENABLE_HANDLER_TRACKING 1

namespace Communication::Serial
{
	SerialClientServer::SerialClientServer(boost::asio::io_service& aIoService, const std::string& portName, uint32_t baudrate): port_name(portName), baudrate(baudrate), serial_port(aIoService), io_service(aIoService), timer(aIoService)
	{
		std::cout << __PRETTY_FUNCTION__ << std::endl;
		boost::system::error_code ec;
		serial_port.open(portName, ec);
		if(ec)
		{
	        std::cout << "error : port_->open() failed...com_port_name=" << portName << ", e=" << ec.message().c_str()
	                  << std::endl;
	        return;
		}

		serial_port.set_option(boost::asio::serial_port_base::baud_rate(baudrate));
		serial_port.set_option(boost::asio::serial_port_base::character_size(8));
		serial_port.set_option(boost::asio::serial_port_base::stop_bits(boost::asio::serial_port_base::stop_bits::one));
		serial_port.set_option(boost::asio::serial_port_base::parity(boost::asio::serial_port_base::parity::none));
		serial_port.set_option(boost::asio::serial_port_base::flow_control(boost::asio::serial_port_base::flow_control::none));
	}

	void SerialClientServer::sendRequest(const SensorMessage& message, std::size_t responseSize, bool hasResponseHeadAndBody)
	{
		sendMessage(boost::asio::mutable_buffer(message.getData(), message.getDataLength()), responseSize, hasResponseHeadAndBody);
	}

	void SerialClientServer::sendRequest(const SensorMessage& message, char delimiter, bool hasResponseHeadAndBody)
	{
		sendMessage(boost::asio::mutable_buffer(message.getData(), message.getDataLength()), delimiter, hasResponseHeadAndBody);
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
    void SerialClientServer::sendMessage(const boost::asio::mutable_buffer& messageBuffer, Type responseSize, bool hasResponseHeadAndBody)
    {
    	std::cout << __PRETTY_FUNCTION__ << ": " << (char*)messageBuffer.data() << std::endl;
    	if(!serial_port.is_open())
    	{
    		//TODO: Throw here or just return?
    		std::cout << "Serial port is not open!" << std::endl;
    		return;
    	}
    	serial_port.async_write_some(messageBuffer, boost::bind(&SerialClientServer::handleWriteComplete<Type>, this,
                responseSize, hasResponseHeadAndBody,
                boost::asio::placeholders::error,
                boost::asio::placeholders::bytes_transferred));
    }

    template<typename Type>
    void SerialClientServer::handleWriteComplete(Type responseSize, bool hasResponseHeadAndBody, const boost::system::error_code& error, std::size_t bytesTransferd)
    {
    	std::cout << __PRETTY_FUNCTION__ << std::endl;
    	if(error)
    	{
    		std::cout << error.message() << std::endl;
    		throw std::runtime_error("Serial write returned a error: " + error.message());
    	}

		timer.expires_from_now(boost::posix_time::seconds(2));
		timer.async_wait(boost::bind(&SerialClientServer::timerExpired, this, _1));

    	if constexpr(std::is_same<Type, char>::value)
		{

    		boost::asio::streambuf aBuffer;
			boost::asio::async_read_until(serial_port, aBuffer, responseSize,
    		boost::bind(&SerialClientServer::handleReceivedResponse, this,
    		                   hasResponseHeadAndBody,
    		                   boost::asio::placeholders::error,
    		                   boost::asio::placeholders::bytes_transferred));

		}
    	else
    	{
    		serial_port.async_read_some(boost::asio::buffer(data, responseSize)
    		, boost::bind(&SerialClientServer::handleReceivedResponse, this,
    		                hasResponseHeadAndBody,
    		                boost::asio::placeholders::error,
    		                boost::asio::placeholders::bytes_transferred));
    	}

    }

    void SerialClientServer::handleReceivedResponse(bool hasResponseHeadAndBody, const boost::system::error_code& error, std::size_t bytesTransferd)
    {
    	boost::system::error_code error_c;
    	timer.cancel(error_c);
    	if(error)
    	{
    		//TODO would we throw when a read error occurs, now only returning.
//    		throw std::runtime_error("Serial read returned a error: " + error.message());
    		return;
    	}
        std::cout << "GOT RESPONSE(" << bytesTransferd << "): " << reinterpret_cast<char*>(data.data()) << std::endl;
        if (responseHandler.use_count() > 0)
        {
            if (hasResponseHeadAndBody)
            {
                std::size_t bodySize = responseHandler->handleResponseHead(data.data(), bytesTransferd);
                handleWriteComplete(bodySize, false, boost::system::error_code(), 0);
            }
            else
            {
            	responseHandler->handleResponse(data.data(), bytesTransferd);
            }
        }
    }

void SerialClientServer::timerExpired(const boost::system::error_code& error)
{
	if(error != boost::asio::error::operation_aborted)
	{
		std::cout << "Timer expired" << std::endl;
		stop();
		//TODO throw here or just close and reopen the port?
		//throw std::runtime_error("Timout occured on serial port.");
	}
}

void SerialClientServer::stop()
{
	if(serial_port.is_open())
	{
		serial_port.cancel();
		serial_port.close();
	}
	io_service.stop();
	io_service.reset();
}

}
