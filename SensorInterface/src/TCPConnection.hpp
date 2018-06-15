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

using boost::asio::ip::tcp;
namespace Communication::TCP
{

class TCPSession : public std::enable_shared_from_this<TCPSession>
{
public:
	TCPSession(boost::asio::io_service& ioService):
		socket(ioService)
	{
	}

	boost::asio::ip::tcp::socket& getSocket()
	{
		return socket;
	}

	void handleIncommingConnection()
	{
		boost::asio::async_read( getSocket(),
			 boost::asio::buffer( data, 5),
			 boost::bind( &TCPSession::handleRequest,
			 shared_from_this(),
			 boost::asio::placeholders::error,
			 boost::asio::placeholders::bytes_transferred));
	}

	void handleOutgoingConnection(const std::string& message)
	{
		std::cout << "STARTED SENDING REQUEST: " << message << std::endl;
		boost::asio::async_write(getSocket(), boost::asio::buffer(message),
			boost::bind(&TCPSession::handleResponse, shared_from_this(),
			boost::asio::placeholders::error,
			boost::asio::placeholders::bytes_transferred));
	}

	void handleResponse(const boost::system::error_code& error,
			std::size_t bytesTransferd )
	{
		std::cout << "Send: " << bytesTransferd << " bytes." << std::endl;
		if(!error)
		{
			boost::asio::async_read( getSocket(),
				 boost::asio::buffer( data, 5),[&](const boost::system::error_code& error, std::size_t bytesRead){std::cout << "RECEIVED RESPONSE: " << data << std::endl;});
		}
		else
		{
			std::cout << "FAILED TO GET RESPONSE: " << error.message() << std::endl;
		}
	}

	void handleRequest(const boost::system::error_code& error,
			std::size_t bytesTransferd)
	{
		//TODO Somehting with calling a callback.
		if(!error)
		{
			std::cout << "We got request with data: " << data << std::endl;
			boost::asio::async_write(getSocket(), boost::asio::buffer(data , 5),[&](const boost::system::error_code& error, std::size_t byteWritten){std::cout << "Written response: " << data << std::endl;});
		}
		else
		{
			std::cout << "INVALID REQUEST RECEIVED: " << error.message() << std::endl;
		}
	}

private:
	boost::asio::ip::tcp::socket socket;
	char data[5];
};

class TCPServerClient : public ConnectionInterface, public std::enable_shared_from_this<TCPServerClient>
{
public:
	TCPServerClient(boost::asio::io_service& aIoService,std::string host, std::string remotePort, std::string localPort):
		host(host),
		remotePort(remotePort),
		localPort(localPort),
		acceptor(aIoService, boost::asio::ip::tcp::endpoint( boost::asio::ip::tcp::v4(), std::atoi(localPort.c_str()))),
		ioService(aIoService)
	{
//		handleAccept( std::make_shared<TCPSession>(TCPSession(ioService)), boost::system::error_code());
		start_accept();
	}

//	TCPServerClient(std::string host, std::string remotePort, std::string localPort):
//		host(host),
//		remotePort(remotePort),
//		localPort(localPort),
//		acceptor(getIOService(), boost::asio::ip::tcp::endpoint( boost::asio::ip::tcp::v4(), std::atoi(localPort.c_str()))),
//		ioService(getIOService())
//	{
//		handleAccept( nullptr, boost::system::error_code());
//	}

	void handleAccept( 	std::shared_ptr<TCPSession> aSession,
						const boost::system::error_code& error)
	{
		std::cout << __PRETTY_FUNCTION__ << std::endl;
		if (!error)
		{
			if(!ioService.stopped())
			{
				if (aSession)
				{
					std::cout << "We gaan een sessie starten" << std::endl;
					aSession->handleIncommingConnection();
				}
				else
				{
					std::cout << "We hebben een binnenkomende verbinding" << std::endl;
					std::shared_ptr<TCPSession> session = std::shared_ptr<TCPSession>(new TCPSession( ioService));

					acceptor.async_accept( session->getSocket(),
							boost::bind( &TCPServerClient::handleAccept, shared_from_this(), session, boost::asio::placeholders::error));
				}
			}
			else
			{
				std::cout << "service stopped" << std::endl;
			}
		} else
		{
			std::cout << "FAILED TO CREATE SERVER SESSION" << std::endl;
			throw std::runtime_error( __PRETTY_FUNCTION__ + std::string( ": ") + error.message());
		}
	}

	void sendRequest(uint8_t* data, std::size_t length)
	{
//		sendMessage(boost::asio::mutable_buffer(data, length));
	}

	void sendRequest(const SensorMessage& message )
	{
//		sendMessage(boost::asio::mutable_buffer(message.getData(), message.getDataLength()));
	}

	void sendRequest(const std::string& message)
	{
//		sendRequest((uint8_t*)&message[0], message.length());
		sendMessage(message);
	}

	virtual~ TCPServerClient()
	{
		acceptor.cancel();
		acceptor.close();
	}
private:
	void start_accept()
	{
		std::shared_ptr<TCPSession> session = std::shared_ptr<TCPSession>(new TCPSession( ioService));

		acceptor.async_accept( session->getSocket(),
		boost::bind( &TCPServerClient::handle_accept, this, session, boost::asio::placeholders::error));
	  }

	  void handle_accept(std::shared_ptr<TCPSession> new_connection,
	      const boost::system::error_code& error)
	  {
	    if (!error)
	    {
	      new_connection->handleIncommingConnection();
	      start_accept();
	    }
	  }

	void sendMessage(const std::string& messageBuffer)
	{
		std::cout << __PRETTY_FUNCTION__ << std::endl;
		std::shared_ptr<TCPSession> session = std::shared_ptr<TCPSession>(new TCPSession(ioService));

		boost::asio::ip::tcp::resolver resolver( ioService);
		boost::asio::ip::tcp::resolver::query query( boost::asio::ip::tcp::v4(), host, remotePort);
		boost::asio::ip::tcp::resolver::iterator endpoint_iterator = resolver.resolve( query);
		boost::asio::ip::tcp::endpoint endpoint = *endpoint_iterator;

		session->getSocket().async_connect( endpoint,
											boost::bind( &TCPServerClient::handleConnect, this, session, messageBuffer, boost::asio::placeholders::error));
	}

	void handleConnect(std::shared_ptr<TCPSession> aSession, const std::string& messageBuffer,
			const boost::system::error_code& error)
	{
		if (!error)
		{
			std::cout << "CONNECTED TO HOST" << std::endl;
			aSession->handleOutgoingConnection(messageBuffer);
		} else
		{
			std::cout << "COULD NOT CONNECT TO HOST: " << error.message() << std::endl;
		}
	}

	std::string host;
	std::string remotePort;
	std::string localPort;

	std::shared_ptr<RequestHandler> requestHandler;
	std::shared_ptr<ResponseHandler> responseHandler;

	boost::asio::ip::tcp::acceptor acceptor;
	boost::asio::io_service& ioService;
};
}

#endif /* SRC_TCPCONNECTION_HPP_ */
