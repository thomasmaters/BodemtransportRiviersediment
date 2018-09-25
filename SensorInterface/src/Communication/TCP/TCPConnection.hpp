/*
 * TCPServer.hpp
 *
 *  Created on: 13 Jun 2018
 *      Author: Thomas Maters
 *		Email : thomasmaters@hotmail.com (TG.Maters@student.han.nl)
 */

#ifndef SRC_TCPCONNECTION_HPP_
#define SRC_TCPCONNECTION_HPP_

#include "ConnectionInterface.hpp"
#include "TCPSession.hpp"

#include <boost/asio/io_service.hpp>
#include <boost/asio/ip/tcp.hpp>

using boost::asio::ip::tcp;
namespace Communication::TCP
{
/**
 * Class handles sending and starting receiving TCP connections.
 */
class TCPServerClient : public ConnectionInterface, public std::enable_shared_from_this<TCPServerClient>
{
  public:
    TCPServerClient(boost::asio::io_service& io_service,
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
    void sendRequest(std::string& message, std::size_t response_size);

    virtual ~TCPServerClient();

  private:
    /**
     * Starts listening on a port.
     */
    void startAccept();

    /**
     * Handles when a new incoming connection has been made.
     * @param new_connection Session.
     * @param error Erros while connecting.
     */
    void handleAccept(std::shared_ptr<TCPSession> new_connection, const boost::system::error_code& error);

    /**
     * Sends a message buffer.
     * @param message_buffer
     * @param response_indentifier
     * @param has_response_head_and_body
     */
    template <typename Type>
    void sendMessage(const boost::asio::mutable_buffer& message_buffer,
                     Type response_indentifier,
                     bool has_response_head_and_body);

    /**
     * Handles sending an outgoing message on an active session.
     * @param message_buffer To send.
     * @param response_indentifier Response size of delimiter.
     * @param has_response_head_and_body Read in 2 parts.
     * @param error Erros while connecting.
     */
    template <typename Type>
    void handleConnect(const boost::asio::mutable_buffer& message_buffer,
                       Type response_indentifier,
                       bool has_response_head_and_body,
                       const boost::system::error_code& error);

    std::string host_;
    std::string remote_port_;
    std::string local_port_;

    boost::asio::ip::tcp::acceptor acceptor_;
    boost::asio::io_service& io_service_;

    std::shared_ptr<TCPSession> active_sesion_;
};
}

#endif /* SRC_TCPCONNECTION_HPP_ */
