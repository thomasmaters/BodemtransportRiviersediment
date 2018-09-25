/*
 * TCPSession.hpp
 *
 *  Created on: 25 Sep 2018
 *      Author: Thomas Maters
 *		Email : thomasmaters@hotmail.com (TG.Maters@student.han.nl)
 */

#ifndef SRC_COMMUNICATION_TCP_TCPSESSION_HPP_
#define SRC_COMMUNICATION_TCP_TCPSESSION_HPP_

#include "../ConnectionInterface.hpp"

#include <boost/asio/io_service.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/placeholders.hpp>
#include <boost/asio/read.hpp>
#include <boost/asio/read_until.hpp>
#include <boost/asio/streambuf.hpp>
#include <boost/asio/write.hpp>
#include <boost/bind.hpp>

#ifdef ENABLE_IO_DEBUG
#include <iostream>
#endif

#define TCP_BUFFER_SIZE 4096

using boost::asio::ip::tcp;
namespace Communication::TCP
{
/**
 *  Class handles a TCP session.
 */
class TCPSession : public std::enable_shared_from_this<TCPSession>
{
  public:
    TCPSession(boost::asio::io_service& io_service, bool keep_alive = false);

    virtual ~TCPSession();

    /**
     * Checks if the socket is connected.
     * @return
     */
    inline bool isOpen()
    {
        return socket_.is_open();
    }

    /**
     * Gets a reference to the socket.
     * @return
     */
    inline boost::asio::ip::tcp::socket& getSocket()
    {
        return socket_;
    }

    /**
     * Handles when a request is being send.
     * @param request_handler To handle the request.
     */
    void handleIncommingConnection(std::shared_ptr<RequestHandler> request_handler);

    /**
     * Handles writing to a remote host.
     * @param message To send.
     * @param response_indentifier Response size or delimiter.
     * @param has_response_head_and_body Read in 2 parts.
     * @param response_handler Users handler to handle the response.
     */
    template <typename Type>
    void handleOutgoingConnection(const boost::asio::mutable_buffer& message,
                                  Type response_indentifier,
                                  bool has_response_head_and_body,
                                  std::shared_ptr<ResponseHandler> response_handler);

    /**
     * Handles reading response on a request.
     * @param response_indentifier Response size or delimiter.
     * @param has_response_head_and_body Read in 2 parts.
     * @param error Errors while sending.
     * @param bytes_transferd Bytes send.
     * @param response_handler Users handler to handle the response.
     */
    template <typename Type>
    void handleResponse(Type response_indentifier,
                        bool has_response_head_and_body,
                        const boost::system::error_code& error,
                        std::size_t bytes_transferd,
                        std::shared_ptr<ResponseHandler> response_handler);

    /**
     * Handles when a response has been read.
     * @param has_response_head_and_body Read in 2 parts.
     * @param error Errors while receiving.
     * @param bytes_transferd Bytes received.
     * @param response_handler Users handler to handle the response.
     */
    void handleReceivedResponse(bool has_response_head_and_body,
                                const boost::system::error_code& error,
                                std::size_t bytes_transferd,
                                std::shared_ptr<ResponseHandler> response_handler);

    /**
     * Handles when a request has been read.
     * @param error Errors while receiving.
     * @param bytes_transferd Bytes received.
     * @param request_handler Users handle to handle the request.
     */
    void handleRequest(const boost::system::error_code& error,
                       std::size_t bytes_transferd,
                       std::shared_ptr<RequestHandler> request_handler);

  private:
    boost::asio::ip::tcp::socket socket_;
    std::array<uint8_t, TCP_BUFFER_SIZE> data_;
    bool keep_alive_;
};
}

#include "TCPSession.inc"

#endif /* SRC_COMMUNICATION_TCP_TCPSESSION_HPP_ */
