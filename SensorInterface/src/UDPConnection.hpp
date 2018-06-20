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
      :
    	  host(host)
      , remotePort(remotePort)
      , localPort(localPort)
      , socketOutgoing(aIoService)
      , socketIncomming(aIoService, udp::endpoint(udp::v4(), std::atoi(localPort.c_str())))
      , service(aIoService)
    {
        start_receive();
    }

	void sendRequest(const SensorMessage& message, std::size_t responseSize, bool hasResponseHeadAndBody = false)
	{
		sendMessage(boost::asio::mutable_buffer(message.getData(), message.getDataLength()), responseSize, hasResponseHeadAndBody);
	}

	void sendRequest(const SensorMessage& message, char delimiter, bool hasResponseHeadAndBody = false)
	{
		throw std::runtime_error("UDP connection do not support reading until a delimiter, use a response size instead.");
	}

	void sendRequest(std::string message, std::size_t responseSize)
	{
		sendMessage(boost::asio::mutable_buffer(message.data(), message.length()), responseSize, false);
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
    bool connectOutgoingSocket()
    {
        boost::asio::ip::udp::resolver resolver(service);
        boost::asio::ip::udp::resolver::query query(boost::asio::ip::udp::v4(), host, remotePort);
        boost::asio::ip::udp::resolver::iterator iter = resolver.resolve(query);
        boost::system::error_code ec;
        boost::asio::connect(socketOutgoing, iter, ec);
        if(ec)
        {
        	std::cout << ec.message() << std::endl;
        }
        return !ec;
    }


    void sendMessage(const boost::asio::mutable_buffer& buffer, std::size_t responseSize, bool hasResponseHeadAndBody)
    {
        if (connectOutgoingSocket())
        {
        	std::cout << __PRETTY_FUNCTION__ << ": " << (char*)buffer.data() << std::endl;
        	socketOutgoing.async_send(buffer,
        			boost::bind(&UDPServerClient::getResponse,
        					this,
							responseSize,
							hasResponseHeadAndBody,
							boost::asio::placeholders::error,
							boost::asio::placeholders::bytes_transferred));
        }
    }

    void getResponse(std::size_t responseSize, bool hasResponseHeadAndBody, const boost::system::error_code& error, std::size_t bytes_transferred)
    {
        std::cout << "WRITTEN: " << bytes_transferred << " bytes" << std::endl;
        if (!error)
        {
                socketOutgoing.async_receive_from(boost::asio::buffer(data, responseSize), remote_endpoint_,
                                                  boost::bind(&UDPServerClient::gotResponse, this, hasResponseHeadAndBody,
                                                              boost::asio::placeholders::error,
                                                              boost::asio::placeholders::bytes_transferred));
        }
        else
        {
            std::cerr << error.message() << std::endl;
        }
    }

    void gotResponse(bool hasResponseHeadAndBody, const boost::system::error_code& error, std::size_t bytes_transferred)
    {
    	if(error)
    	{
    		//throw std::runtime_error("Failed getting response." + error.message());
    		return;
    	}

        if (responseHandler.use_count() != 0)
        {
            std::cout << "GOT RESPONSE OF: " << bytes_transferred << " bytes" << std::endl;
        	if (hasResponseHeadAndBody)
        	{
        		std::size_t bodyLength = responseHandler->handleResponseHead(data.data(), bytes_transferred);
        		getResponse(bodyLength, false, boost::system::error_code(), 0);
        	}
        	else
        	{
                responseHandler->handleResponse(data.data(), bytes_transferred);
        	}
        }
    }

    void start_receive()
    {
        socketIncomming.async_receive_from(boost::asio::buffer(data, 2), local_endpoint_,
                                           boost::bind(&UDPServerClient::handle_receive, this,
                                                       boost::asio::placeholders::error,
                                                       boost::asio::placeholders::bytes_transferred));
    }

    void handle_receive(const boost::system::error_code& error, std::size_t bytes_transferred)
    {
        std::cout << "Received: " << bytes_transferred << " bytes" << std::endl;
        if (!error || error == boost::asio::error::eof)
        {
            if (requestHandler.use_count() != 0)
            {
                requestHandler->handleRequest(data.data(), bytes_transferred);
            }

            socketIncomming.async_send_to(boost::asio::buffer(data, bytes_transferred), local_endpoint_,
                                          boost::bind(&UDPServerClient::handle_send, this,
                                                      boost::asio::placeholders::error,
                                                      boost::asio::placeholders::bytes_transferred));

            start_receive();
        }
    }

    void handle_send(const boost::system::error_code& error, std::size_t bytes_transferred)
    {
        std::cout << "Send response of: " << bytes_transferred << " bytes" << std::endl;
    }

    std::array<uint8_t, 2048> data;
    boost::asio::streambuf streamBuffer;

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
