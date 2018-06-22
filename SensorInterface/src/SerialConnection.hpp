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

#include <boost/asio/io_service.hpp>
#include <boost/asio/buffer.hpp>
#include <boost/asio/serial_port.hpp>
#include <boost/asio/deadline_timer.hpp>

namespace Communication::Serial
{

class SerialClientServer: public Communication::ConnectionInterface, public std::enable_shared_from_this<SerialClientServer>
{
public:
	SerialClientServer(boost::asio::io_service& aIoService, const std::string& portName, uint32_t baudrate);

	void sendRequest(const SensorMessage& message, std::size_t responseSize, bool hasResponseHeadAndBody = false);

	void sendRequest(const SensorMessage& message, char delimiter, bool hasResponseHeadAndBody = false);

	void sendRequest(std::string message, std::size_t responseSize);

	virtual ~SerialClientServer();

private:
    template <typename Type>
    void sendMessage(const boost::asio::mutable_buffer& messageBuffer, Type responseSize, bool hasResponseHeadAndBody);

    template<typename Type>
    void handleWriteComplete(Type responseSize, bool hasResponseHeadAndBody, const boost::system::error_code& error, std::size_t bytesTransferd);

    void handleReceivedResponse(bool hasResponseHeadAndBody, const boost::system::error_code& error, std::size_t bytesTransferd);

    void timerExpired(const boost::system::error_code& error);

    void stop();

	std::string port_name;
	uint32_t baudrate;

	boost::asio::serial_port serial_port;
	boost::asio::io_service& io_service;

	boost::asio::deadline_timer timer;
	std::optional<boost::system::error_code> timer_result;

	std::array<uint8_t, 2048> data;
};

}

#endif /* SRC_SERIALCONNECTION_HPP_ */
