/*
 * UDPServer.hpp
 *
 *  Created on: 13 Jun 2018
 *      Author: Thomas Maters
 *		Email : thomasmaters@hotmail.com (TG.Maters@student.han.nl)
 */

#ifndef SRC_UDPCONNECTION_HPP_
#define SRC_UDPCONNECTION_HPP_

#include "ConnectionInterface.hpp"

using boost::asio::ip::udp;
namespace Communication::UDP
{
class UDPServerClient : public ConnectionInterface, public std::enable_shared_from_this<UDPServerClient>
{
  public:
    UDPServerClient(boost::asio::io_service& aIoService, std::string host, std::string remotePort,
                    std::string localPort)
      : host(host)
      , remotePort(remotePort)
      , localPort(localPort)
      , socketOutgoing(aIoService, udp::endpoint(udp::v4(), std::atoi(remotePort.c_str())))
      , socketIncomming(aIoService, udp::endpoint(udp::v4(), std::atoi(localPort.c_str())))
      , service(aIoService)
    {
        start_receive();
    }

    void sendRequest(uint8_t* data, std::size_t length)
    {
        sendMessage(boost::asio::mutable_buffer(reinterpret_cast<char*>(data), length));
    }

    void sendRequest(const SensorMessage& message)
    {
        sendMessage(boost::asio::mutable_buffer(message.getData(), message.getDataLength()));
    }

    void sendRequest(const std::string& message)
    {
        sendRequest((uint8_t*)&message[0], message.length());
    }

    void addRequestHandler(std::shared_ptr<RequestHandler> aRequestHandler)
    {
        requestHandler.reset();
        requestHandler.swap(aRequestHandler);
    }

    void addResponseHandler(std::shared_ptr<ResponseHandler> aResponseHandler)
    {
        responseHandler.reset();
        responseHandler.swap(aResponseHandler);
    }

    virtual ~UDPServerClient()
    {
        std::cout << __PRETTY_FUNCTION__ << std::endl;
    }

  private:
    void sendMessage(const boost::asio::mutable_buffer& buffer)
    {
        boost::asio::ip::udp::resolver resolver(service);
        boost::asio::ip::udp::resolver::query query(boost::asio::ip::udp::v4(), host, remotePort);
        boost::asio::ip::udp::resolver::iterator iter = resolver.resolve(query);
        remote_endpoint_ = *iter;

        std::cout << __PRETTY_FUNCTION__ << ": " << (char*)buffer.data() << std::endl;
        socketOutgoing.async_send_to(buffer, remote_endpoint_,
                              boost::bind(&UDPServerClient::getResponse, this, boost::asio::placeholders::error,
                                          boost::asio::placeholders::bytes_transferred));
    }

    void getResponse(const boost::system::error_code& error, std::size_t bytes_transferred)
    {
        std::cout << "WRITTEN: " << bytes_transferred << " bytes" << std::endl;
        socketOutgoing.async_receive_from(boost::asio::buffer(data, 5), remote_endpoint_,
                                   boost::bind(&UDPServerClient::gotResponse, this, boost::asio::placeholders::error,
                                               boost::asio::placeholders::bytes_transferred));
    }

    void gotResponse(const boost::system::error_code& error, std::size_t bytes_transferred)
    {
        if (responseHandler.use_count() != 0)
        {
            std::cout << "GOT RESPONSE OF: " << bytes_transferred << " bytes" << std::endl;
            responseHandler->handleResponse((uint8_t*)data, 5);
        }
    }

    void start_receive()
    {
        socketIncomming.async_receive_from(boost::asio::buffer(data, 5), local_endpoint_,
                                    boost::bind(&UDPServerClient::handle_receive, this,
                                                boost::asio::placeholders::error,
                                                boost::asio::placeholders::bytes_transferred));
    }

    void handle_receive(const boost::system::error_code& error, std::size_t bytes_transferred)
    {
        std::cout << "Received: " << bytes_transferred << " bytes" << std::endl;
        if (!error)
        {
            if (requestHandler.use_count() != 0)
            {
                requestHandler->handleRequest((uint8_t*)data, 5);
            }

            socketIncomming.async_send_to(boost::asio::buffer(data, 5), local_endpoint_,
                                   boost::bind(&UDPServerClient::handle_send, this, boost::asio::placeholders::error,
                                               boost::asio::placeholders::bytes_transferred));

            start_receive();
        }
    }

    void handle_send(const boost::system::error_code& error, std::size_t bytes_transferred)
    {
        std::cout << "Send response of: " << bytes_transferred << " bytes" << std::endl;
    }

    char data[5];

    std::string host;
    std::string remotePort;
    std::string localPort;

    std::shared_ptr<RequestHandler> requestHandler;
    std::shared_ptr<ResponseHandler> responseHandler;

    boost::asio::ip::udp::endpoint local_endpoint_;
    boost::asio::ip::udp::endpoint remote_endpoint_;

    boost::asio::ip::udp::socket socketOutgoing;
    boost::asio::ip::udp::socket socketIncomming;

    boost::asio::io_service& service;
};
}

#endif /* SRC_UDPCONNECTION_HPP_ */
