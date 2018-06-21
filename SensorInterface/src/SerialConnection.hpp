/*
 * SerialServer.hpp
 *
 *  Created on: 13 Jun 2018
 *      Author: Thomas Maters
 *		Email : thomasmaters@hotmail.com (TG.Maters@student.han.nl)
 */

#ifndef SRC_SERIALCONNECTION_HPP_
#define SRC_SERIALCONNECTION_HPP_

#include "ConnectionInterface.hpp"

namespace Communication::Serial
{

class SerialClientServer: public Communication::ConnectionInterface, public std::enable_shared_from_this<SerialClientServer>
{
public:
	SerialClientServer(boost::asio::io_service& aIoService, const std::string& portName, uint32_t baudrate): portName(portName), baudrate(baudrate), serialPort(aIoService), io_service_(aIoService)
	{
		std::cout << __PRETTY_FUNCTION__ << std::endl;
		boost::system::error_code ec;
		serialPort.open(portName, ec);
		if(ec)
		{
	        std::cout << "error : port_->open() failed...com_port_name=" << portName << ", e=" << ec.message().c_str()
	                  << std::endl;
	        return;
		}

		serialPort.set_option(boost::asio::serial_port_base::baud_rate(baudrate));
		serialPort.set_option(boost::asio::serial_port_base::character_size(8));
		serialPort.set_option(boost::asio::serial_port_base::stop_bits(boost::asio::serial_port_base::stop_bits::one));
		serialPort.set_option(boost::asio::serial_port_base::parity(boost::asio::serial_port_base::parity::none));
		serialPort.set_option(boost::asio::serial_port_base::flow_control(boost::asio::serial_port_base::flow_control::none));

		std::cout << "Hallo" << std::endl;

//	    std::thread t(
//	        boost::bind(static_cast<size_t (boost::asio::io_service::*)()>(&boost::asio::io_service::run), &io_service_));
	}

	void sendRequest(const SensorMessage& message, std::size_t responseSize, bool hasResponseHeadAndBody = false)
	{
		sendMessage(boost::asio::mutable_buffer(message.getData(), message.getDataLength()), responseSize, hasResponseHeadAndBody);
	}

	void sendRequest(const SensorMessage& message, char delimiter, bool hasResponseHeadAndBody = false)
	{
		sendMessage(boost::asio::mutable_buffer(message.getData(), message.getDataLength()), delimiter, hasResponseHeadAndBody);
	}

	void sendRequest(std::string message, std::size_t responseSize)
	{
		sendMessage(boost::asio::mutable_buffer(message.data(), message.length()), responseSize, false);
	}

	virtual ~SerialClientServer()
	{
		std::cout << __PRETTY_FUNCTION__ << std::endl;
		if(serialPort.is_open())
		{
			serialPort.cancel();
			serialPort.close();
		}
		io_service_.stop();
		io_service_.reset();
	}

private:
    template <typename Type>
    void sendMessage(const boost::asio::mutable_buffer& messageBuffer, Type responseSize, bool hasResponseHeadAndBody)
    {
    	std::cout << __PRETTY_FUNCTION__ << ": " << (char*)messageBuffer.data() << std::endl;
    	if(!serialPort.is_open())
    	{
    		std::cout << "Serial port is not open!" << std::endl;
    	}
    	serialPort.async_write_some(messageBuffer, boost::bind(&SerialClientServer::handleWriteComplete<Type>, this,
                responseSize, hasResponseHeadAndBody,
                boost::asio::placeholders::error,
                boost::asio::placeholders::bytes_transferred));
//	boost::asio::async_write(serialPort, messageBuffer, [&](const boost::system::error_code& error, std::size_t bytesTransferred){std::cout << "send some" << std::endl;});
//    	serialPort.async_write_some(messageBuffer, [&](const boost::system::error_code& error, std::size_t bytesTransferred){std::cout << "send some" << std::endl;});
    }

    template<typename Type>
    void handleWriteComplete(Type responseSize, bool hasResponseHeadAndBody, const boost::system::error_code& error, std::size_t bytesTransferd)
    {
    	std::cout << __PRETTY_FUNCTION__ << std::endl;
    	if(error)
    	{
    		std::cout << error.message() << std::endl;
    	}
    	if constexpr(std::is_same<Type, char>::value)
		{
    	 boost::asio::streambuf aBuffer;
        	boost::asio::async_read_until(serialPort, aBuffer, responseSize, boost::bind(&SerialClientServer::handleReceivedResponse, this,
                    hasResponseHeadAndBody,
                    boost::asio::placeholders::error,
                    boost::asio::placeholders::bytes_transferred));
		}
    	else
    	{
    		serialPort.async_read_some(boost::asio::buffer(data, responseSize), boost::bind(&SerialClientServer::handleReceivedResponse, this,
                    hasResponseHeadAndBody,
                    boost::asio::placeholders::error,
                    boost::asio::placeholders::bytes_transferred));
    	}
    }

    void handleReceivedResponse(bool hasResponseHeadAndBody, const boost::system::error_code& error,
                                std::size_t bytesTransferd)
    {
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

//    void start_receive()
//    {
//        serialPort.(boost::asio::buffer(data, 2), local_endpoint_,
//                                           boost::bind(&SerialClientServer::handle_receive, this,
//                                                       boost::asio::placeholders::error,
//                                                       boost::asio::placeholders::bytes_transferred));
//    }
//
//    void handle_receive(const boost::system::error_code& error, std::size_t bytes_transferred)
//    {
//        std::cout << "Received: " << bytes_transferred << " bytes" << std::endl;
//        if (!error || error == boost::asio::error::eof)
//        {
//            if (requestHandler.use_count() != 0)
//            {
//                requestHandler->handleRequest(data.data(), bytes_transferred);
//            }
//
//            serialPort.async_send_to(boost::asio::buffer(data, bytes_transferred), local_endpoint_,
//                                          boost::bind(&SerialClientServer::handle_send, this,
//                                                      boost::asio::placeholders::error,
//                                                      boost::asio::placeholders::bytes_transferred));
//
//            start_receive();
//        }
//    }

	std::string portName;
	uint32_t baudrate;

	boost::asio::serial_port serialPort;
	boost::asio::io_service& io_service_;

	std::array<uint8_t, 2048> data;
};

}

#endif /* SRC_SERIALCONNECTION_HPP_ */
