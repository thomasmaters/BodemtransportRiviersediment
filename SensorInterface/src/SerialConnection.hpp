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

#include <boost/asio/buffer.hpp>
#include <boost/asio/deadline_timer.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/asio/serial_port.hpp>

#define SERIAL_BUFFER_SIZE 2048

namespace Communication::Serial
{
class SerialClientServer : public Communication::ConnectionInterface,
                           public std::enable_shared_from_this<SerialClientServer>
{
  public:
    SerialClientServer(boost::asio::io_service& io_service, const std::string& port_name, uint32_t baudrate);

    void sendRequest(const SensorMessage& message, std::size_t response_size, bool has_response_head_and_body = false);

    void sendRequest(const SensorMessage& message, char delimiter, bool has_response_head_and_body = false);

    void sendRequest(std::string message, std::size_t response_size);

    virtual ~SerialClientServer();

  private:
    template <typename Type>
    void sendMessage(const boost::asio::mutable_buffer& message_buffer,
                     Type response_indentifier,
                     bool has_response_head_and_body);

    template <typename Type>
    void handleWriteComplete(Type response_indentifier,
                             bool has_response_head_and_body,
                             const boost::system::error_code& error,
                             std::size_t bytes_transferd);

    void handleReceivedResponse(bool has_response_head_and_body,
                                const boost::system::error_code& error,
                                std::size_t bytes_transferd);

    void timerExpired(const boost::system::error_code& error);

    void stop();

    std::string port_name_;
    uint32_t baudrate_;

    boost::asio::serial_port serial_port_;
    boost::asio::io_service& io_service_;

    boost::asio::deadline_timer timer_;
    std::optional<boost::system::error_code> timer_result_;

    std::array<uint8_t, SERIAL_BUFFER_SIZE> data_;
};
}

#endif /* SRC_SERIALCONNECTION_HPP_ */
