/*
 * TCPServer.hpp
 *
 *  Created on: 13 Jun 2018
 *      Author: Thomas Maters
 *		Email : thomasmaters@hotmail.com (TG.Maters@student.han.nl)
 */

#ifndef SRC_TCPCONNECTION_HPP_
#define SRC_TCPCONNECTION_HPP_

#include <boost/asio.hpp>
#include <boost/bind.hpp>

#include "ConnectionInterface.hpp"

#define TCP_BUFFER_SIZE 1024

using boost::asio::ip::tcp;
namespace Communication::TCP
{
class TCPSession : public std::enable_shared_from_this<TCPSession>
{
  public:
    TCPSession(boost::asio::io_service& ioService) : socket(ioService)
    {
    }

    boost::asio::ip::tcp::socket& getSocket()
    {
        return socket;
    }

    void handleIncommingConnection(std::shared_ptr<RequestHandler> aRequestHandler)
    {
        boost::asio::async_read(getSocket(), boost::asio::buffer(data, 5),
                                boost::bind(&TCPSession::handleRequest, shared_from_this(),
                                            boost::asio::placeholders::error,
                                            boost::asio::placeholders::bytes_transferred, aRequestHandler));
    }

    template <typename Type>
    void handleOutgoingConnection(const boost::asio::mutable_buffer& message, Type responseSize,
                                  bool hasResponseHeadAndBody, std::shared_ptr<ResponseHandler> aResponseHandler)
    {
        std::cout << "STARTED SENDING REQUEST: " << (char*)message.data() << std::endl;
        boost::asio::async_write(getSocket(), boost::asio::buffer(message),
                                 boost::bind(&TCPSession::handleResponse<Type>, shared_from_this(), responseSize,
                                             hasResponseHeadAndBody, boost::asio::placeholders::error,
                                             boost::asio::placeholders::bytes_transferred, aResponseHandler));
    }

    template <typename Type>
    void handleResponse(Type responseSize, bool hasResponseHeadAndBody, const boost::system::error_code& error,
                        std::size_t bytesTransferd, std::shared_ptr<ResponseHandler> aResponseHandler)
    {
        std::cout << "Send: " << bytesTransferd << " bytes." << std::endl;
        if (!error)
        {
            boost::asio::streambuf aBuffer;
            if
                constexpr(std::is_same<Type, char>::value)
                {
                    boost::asio::async_read_until(getSocket(), aBuffer, responseSize,
                                                  boost::bind(&TCPSession::handleReceivedResponse, shared_from_this(),
                                                              hasResponseHeadAndBody, boost::asio::placeholders::error,
                                                              boost::asio::placeholders::bytes_transferred,
                                                              aResponseHandler));
                }
            else
            {
                boost::asio::async_read(getSocket(), boost::asio::buffer(data, responseSize),
                                        boost::bind(&TCPSession::handleReceivedResponse, shared_from_this(),
                                                    hasResponseHeadAndBody, boost::asio::placeholders::error,
                                                    boost::asio::placeholders::bytes_transferred, aResponseHandler));
            }
        }
        else
        {
            std::cout << "FAILED TO GET RESPONSE: " << error.message() << std::endl;
        }
    }
    void handleReceivedResponse(bool hasResponseHeadAndBody, const boost::system::error_code& error,
                                std::size_t bytesTransferd, std::shared_ptr<ResponseHandler> aResponseHandler)
    {
        std::cout << "GOT RESPONSE(" << bytesTransferd << "): " << data << std::endl;
        if (aResponseHandler.use_count() > 0)
        {
            if (hasResponseHeadAndBody)
            {
                std::size_t bodySize =
                    aResponseHandler->handleResponseHead(reinterpret_cast<uint8_t*>(data), bytesTransferd);
                handleResponse(bodySize, false, boost::system::error_code(), 0, aResponseHandler);
            }
            else
            {
                aResponseHandler->handleResponse((uint8_t*)data, bytesTransferd);
            }
        }
    }

    void handleRequest(const boost::system::error_code& error, std::size_t bytesTransferd,
                       std::shared_ptr<RequestHandler> aRequestHandler)
    {
        if (!error || error == boost::asio::error::eof)
        {
            std::cout << "We got request with data: " << data << std::endl;
            if (aRequestHandler.use_count() > 0)
            {
                aRequestHandler->handleRequest((uint8_t*)data, bytesTransferd);
            }
            boost::asio::async_write(getSocket(), boost::asio::buffer(data, 5),
                                     [&](const boost::system::error_code& error, std::size_t byteWritten) {
                                         std::cout << "Written response: " << data << std::endl;
                                     });
        }
        else
        {
            std::cout << "INVALID REQUEST RECEIVED: " << error.message() << std::endl;
        }
    }

  private:
    boost::asio::ip::tcp::socket socket;
    char data[TCP_BUFFER_SIZE];
};

class TCPServerClient : public ConnectionInterface, public std::enable_shared_from_this<TCPServerClient>
{
  public:
    TCPServerClient(boost::asio::io_service& aIoService, std::string host, std::string remotePort,
                    std::string localPort)
      : host(host)
      , remotePort(remotePort)
      , localPort(localPort)
      , acceptor(aIoService, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), std::atoi(localPort.c_str())))
      , ioService(aIoService)
    {
        start_accept();
    }

    void sendRequest(const SensorMessage& message, std::size_t responseSize, bool hasResponseHeadAndBody = false)
    {
        sendMessage(boost::asio::mutable_buffer(message.getData(), message.getDataLength()), responseSize,
                    hasResponseHeadAndBody);
    }

    void sendRequest(const SensorMessage& message, char delimiter, bool hasResponseHeadAndBody = false)
    {
        sendMessage(boost::asio::mutable_buffer(message.getData(), message.getDataLength()), delimiter, false);
    }

    void sendRequest(std::string message, std::size_t responseSize)
    {
        sendMessage(boost::asio::mutable_buffer(message.data(), message.length()), responseSize, false);
    }

    virtual ~TCPServerClient()
    {
        acceptor.cancel();
        acceptor.close();
    }

  private:
    void start_accept()
    {
        std::shared_ptr<TCPSession> session = std::shared_ptr<TCPSession>(new TCPSession(ioService));

        acceptor.async_accept(session->getSocket(), boost::bind(&TCPServerClient::handle_accept, this, session,
                                                                boost::asio::placeholders::error));
    }

    void handle_accept(std::shared_ptr<TCPSession> new_connection, const boost::system::error_code& error)
    {
        if (!error)
        {
            new_connection->handleIncommingConnection(requestHandler);
            start_accept();
        }
    }

    template <typename Type>
    void sendMessage(const boost::asio::mutable_buffer& messageBuffer, Type responseSize, bool hasResponseHeadAndBody)
    {
        static_assert(std::is_same<Type, char>::value || std::is_same<Type, std::size_t>::value,
                      "Message responsetype has to be char or std::size_t.");
        std::cout << __PRETTY_FUNCTION__ << std::endl;
        std::shared_ptr<TCPSession> session = std::shared_ptr<TCPSession>(new TCPSession(ioService));

        boost::asio::ip::tcp::resolver resolver(ioService);
        boost::asio::ip::tcp::resolver::query query(boost::asio::ip::tcp::v4(), host, remotePort);
        boost::asio::ip::tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);
        boost::asio::ip::tcp::endpoint endpoint = *endpoint_iterator;

        session->getSocket().async_connect(endpoint, boost::bind(&TCPServerClient::handleConnect<Type>, this, session,
                                                                 messageBuffer, responseSize, hasResponseHeadAndBody,
                                                                 boost::asio::placeholders::error));
    }

    template <typename Type>
    void handleConnect(std::shared_ptr<TCPSession> aSession, const boost::asio::mutable_buffer& messageBuffer,
                       Type responseSize, bool hasResponseHeadAndBody, const boost::system::error_code& error)
    {
        if (!error)
        {
            std::cout << "CONNECTED TO HOST" << std::endl;
            aSession->handleOutgoingConnection(messageBuffer, responseSize, hasResponseHeadAndBody, responseHandler);
        }
        else
        {
            std::cout << "COULD NOT CONNECT TO HOST: " << error.message() << std::endl;
        }
    }

    std::string host;
    std::string remotePort;
    std::string localPort;

    boost::asio::ip::tcp::acceptor acceptor;
    boost::asio::io_service& ioService;
};
}

#endif /* SRC_TCPCONNECTION_HPP_ */
