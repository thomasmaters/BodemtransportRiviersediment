/*
 * UDPConnection.hpp
 *
 *  Created on: 13 Jun 2018
 *      Author: Thomas Maters
 *		Email : thomasmaters@hotmail.com (TG.Maters@student.han.nl)
 */

#ifndef SRC_UDPCONNECTION_HPP_
#define SRC_UDPCONNECTION_HPP_

#include "ConnectionInterface.hpp"
#include "SensorMessage.hpp"

#include <boost/asio/streambuf.hpp>
#include <boost/asio/ip/udp.hpp>
#include <boost/asio/io_service.hpp>

using boost::asio::ip::udp;
namespace Communication::UDP
{
class UDPServerClient : public ConnectionInterface, public std::enable_shared_from_this<UDPServerClient>
{
  public:
    UDPServerClient(boost::asio::io_service& io_service, std::string host, std::string remote_port, std::string local_port);

	void sendRequest(const SensorMessage& message, std::size_t response_size, bool has_response_head_and_body = false);

	void sendRequest(const SensorMessage& message, char delimiter, bool has_response_head_and_body = false);

	void sendRequest(std::string message, std::size_t response_size);

    virtual ~UDPServerClient();

  private:
    bool connectOutgoingSocket();

    void sendMessage(const boost::asio::mutable_buffer& buffer, std::size_t response_size, bool has_response_head_and_body);

    void getResponse(std::size_t response_size, bool has_response_head_and_body, const boost::system::error_code& error, std::size_t bytes_transferred);

    void gotResponse(bool has_response_head_and_body, const boost::system::error_code& error, std::size_t bytes_transferred);

    void start_receive();

    void handle_receive(const boost::system::error_code& error, std::size_t bytes_transferred);

    void handle_send(const boost::system::error_code& error, std::size_t bytes_transferred);

    std::array<uint8_t, 2048> data_;
    boost::asio::streambuf stream_buffer_;

    std::string host_;
    std::string remote_port_;
    std::string local_port_;

    boost::asio::ip::udp::endpoint local_endpoint_;
    boost::asio::ip::udp::endpoint remote_endpoint_;

    boost::asio::ip::udp::socket socket_outgoing_;
    boost::asio::ip::udp::socket socket_incomming_;

    boost::asio::io_service& io_service_;
};
}

#endif /* SRC_UDPCONNECTION_HPP_ */
