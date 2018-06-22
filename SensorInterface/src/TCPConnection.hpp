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

#include <boost/asio/io_service.hpp>
#include <boost/asio/ip/tcp.hpp>


#define TCP_BUFFER_SIZE 1024

using boost::asio::ip::tcp;
namespace Communication::TCP
{
class TCPSession : public std::enable_shared_from_this<TCPSession>
{
  public:
    TCPSession(boost::asio::io_service& io_service);

    inline boost::asio::ip::tcp::socket& getSocket()
    {
        return socket_;
    }

    void handleIncommingConnection(std::shared_ptr<RequestHandler> request_handler);

    template <typename Type>
    void handleOutgoingConnection(const boost::asio::mutable_buffer& message, Type response_indentifier, bool has_response_head_and_body, std::shared_ptr<ResponseHandler> response_handler);

    template <typename Type>
    void handleResponse(Type response_indentifier, bool has_response_head_and_body, const boost::system::error_code& error, std::size_t bytes_transferd, std::shared_ptr<ResponseHandler> response_handler);

    void handleReceivedResponse(bool has_response_head_and_body, const boost::system::error_code& error, std::size_t bytes_transferd, std::shared_ptr<ResponseHandler> response_handler);

    void handleRequest(const boost::system::error_code& error, std::size_t bytes_transferd, std::shared_ptr<RequestHandler> request_handler);

  private:
    boost::asio::ip::tcp::socket socket_;
    char data_[TCP_BUFFER_SIZE];
};

class TCPServerClient : public ConnectionInterface, public std::enable_shared_from_this<TCPServerClient>
{
  public:
    TCPServerClient(boost::asio::io_service& io_service, std::string host, std::string remote_port, std::string local_port);

    void sendRequest(const SensorMessage& message, std::size_t response_size, bool has_response_head_and_body = false);

    void sendRequest(const SensorMessage& message, char delimiter, bool has_response_head_and_body = false);

    void sendRequest(std::string& message, std::size_t response_size);

    virtual ~TCPServerClient();

  private:
    void start_accept();

    void handle_accept(std::shared_ptr<TCPSession> new_connection, const boost::system::error_code& error);

    template <typename Type>
    void sendMessage(const boost::asio::mutable_buffer& message_buffer, Type response_indentifier, bool has_response_head_and_body);

    template <typename Type>
    void handleConnect(std::shared_ptr<TCPSession> session, const boost::asio::mutable_buffer& message_buffer, Type response_indentifier, bool has_response_head_and_body, const boost::system::error_code& error);

    std::string host_;
    std::string remote_port_;
    std::string local_port_;

    boost::asio::ip::tcp::acceptor acceptor_;
    boost::asio::io_service& io_service_;
};
}

#endif /* SRC_TCPCONNECTION_HPP_ */
