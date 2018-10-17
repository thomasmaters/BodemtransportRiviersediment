/*
 * UDPConnection.hpp
 *
 *  Created on: 13 Jun 2018
 *      Author: Thomas Maters
 *		Email : thomasmaters@hotmail.com (TG.Maters@student.han.nl)
 */

#ifndef SRC_UDPCONNECTION_HPP_
#define SRC_UDPCONNECTION_HPP_

#include "../../Messages/SensorMessage.hpp"
#include "../ConnectionInterface.hpp"

#include <boost/asio/io_service.hpp>
#include <boost/asio/ip/udp.hpp>

#define UDP_BUFFER_SIZE 4096

using boost::asio::ip::udp;
namespace Communication::UDP
{
/**
 * Class handles communicating over UDP.
 */
class UDPServerClient : public ConnectionInterface
{
  public:
    UDPServerClient(boost::asio::io_service& io_service,
                    const std::string& host,
                    const std::string& remote_port,
                    const std::string& local_port);

    /**
     * Sends a message.
     * @param message To send.
     * @param response_size Size of bytes to read into response.
     * @param has_response_head_and_body Read in 2 parts.
     */
    void sendRequest(const SensorMessage& message, std::size_t response_size, bool has_response_head_and_body = false);

    /**
     * Sends a message.
     * @param message To send.
     * @param delimiter Read till delimeter found.
     * @param has_response_head_and_body Read in 2 parts.
     */
    void sendRequest(const SensorMessage& message, char delimiter, bool has_response_head_and_body = false);

    /**
     * Sends a message from a string.
     * @param message To send.
     * @param response_size Size of bytes to read.
     */
    void sendRequest(std::string message, std::size_t response_size);

    virtual ~UDPServerClient();

  private:
    /**
     * Connect to outgoing ip.
     * @return True if connected.
     */
    bool connectOutgoingSocket();

    /**
     * Sends a message.
     * @param buffer
     * @param response_size
     * @param has_response_head_and_body
     */
    void sendMessage(const boost::asio::mutable_buffer& buffer,
                     std::size_t response_size,
                     bool has_response_head_and_body);

    /**
     * Handler when ready to read response.
     * @param response_size
     * @param has_response_head_and_body
     * @param error
     * @param bytes_transferred
     */
    void getResponse(std::size_t response_size,
                     bool has_response_head_and_body,
                     const boost::system::error_code& error,
                     [[maybe_unused]] std::size_t bytes_transferred);

    /**
     * Handler when response has been read.
     * @param has_response_head_and_body
     * @param error
     * @param bytes_transferred
     */
    void gotResponse(bool has_response_head_and_body,
                     const boost::system::error_code& error,
                     std::size_t bytes_transferred);

    /**
     * Starts listening on port.
     */
    void startReceive();

    /**
     * Received a message on a port.
     * @param error
     * @param bytes_transferred
     */
    void handleReceive(const boost::system::error_code& error, std::size_t bytes_transferred);

    /**
     * Handle when response has been send on a message.
     * @param error
     * @param bytes_transferred
     */
    void handleSend([[maybe_unused]] const boost::system::error_code& error, std::size_t bytes_transferred);

    std::array<uint8_t, UDP_BUFFER_SIZE> data_;

    std::string host_;
    std::string remote_port_;
    std::string local_port_;

    boost::asio::io_service& io_service_;

    boost::asio::ip::udp::endpoint local_endpoint_;
    boost::asio::ip::udp::endpoint remote_endpoint_;

    boost::asio::ip::udp::socket socket_outgoing_;
    boost::asio::ip::udp::socket socket_incomming_;
};
}

#endif /* SRC_UDPCONNECTION_HPP_ */
