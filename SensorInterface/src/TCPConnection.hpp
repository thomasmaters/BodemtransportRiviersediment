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
#include "ServerInterface.hpp"

using boost::asio::ip::tcp;
namespace Communication::TCP
{

class TCPSession : public std::enable_shared_from_this<TCPSession>
{
public:
	TCPSession(boost::asio::io_service& ioService):
		socket(ioService)
	{
		data[0] = 'K';
		data[1] = 'A';
		data[2] = 'A';
		data[3] = 'S';
	}

	boost::asio::ip::tcp::socket& getSocket()
	{
		return socket;
	}

	void handleIncommingConnection(const boost::system::error_code& error, std::size_t bytes_transferred)
	{
		if(!error)
		{
			boost::asio::async_read( socket,
				 boost::asio::buffer( data, 4),
				 boost::bind( &TCPSession::send,
				 shared_from_this(),
				 boost::asio::placeholders::error,
		         boost::asio::placeholders::bytes_transferred));
		}
		else
		{
			std::cout << "INCOMMING CONNECTION ABORTED" << std::endl;
		}
	}

	void handleOutgoingConnection(std::shared_ptr<TCPSession> aSession,
			const boost::system::error_code& error,
			std::size_t bytesTransferd )
	{
		if(!error)
		{

		}
		else
		{

		}
	}

	void start()
	  {
		 std::cout << __PRETTY_FUNCTION__ << std::endl;
		    boost::asio::async_write(socket, boost::asio::buffer(data,4),
		        boost::bind(&TCPSession::handle_read, shared_from_this(),
		          boost::asio::placeholders::error,
		          boost::asio::placeholders::bytes_transferred));
//			boost::asio::async_read( socket,
//				 boost::asio::buffer( data, 4),
//				 boost::bind( &TCPSession::send,
//				 shared_from_this(),
//				 boost::asio::placeholders::error));
	  }

	 void handle_read(const boost::system::error_code& err,
	                   size_t bytes_transferred)
	  {
		 std::cout << __PRETTY_FUNCTION__ << std::endl;
	    if (!err) {
			boost::asio::async_read( socket,
				 boost::asio::buffer( data, 4),
				 boost::bind( &TCPSession::send,
				 shared_from_this(),
				 boost::asio::placeholders::error));
	    } else {
	      std::cerr << "Fout bij het lezen " << err.message() << std::endl;
	    }
	  }

	 void send(const boost::system::error_code& err)

	 {
		 if(!err)
		 {
			 		    boost::asio::async_write(socket, boost::asio::buffer(data,4),
			 		        boost::bind(&TCPSession::handle_read, shared_from_this(),
			 		          boost::asio::placeholders::error,
			 		          boost::asio::placeholders::bytes_transferred));
			 std::cout << data << std::endl;
			 start();
		 }
		 else
		 {
			 std::cout << err.message() << __PRETTY_FUNCTION__ << std::endl;
			 delete this;
		 }

	 }

	 void handle_send(const boost::system::error_code& err)
	 {

	 }
private:
	boost::asio::ip::tcp::socket socket;
	char data[24];
};


//class TCPServer
//{
//public:
//	TCPServer(uint16_t port = 1337):
//		requestHandler(),
//		responseHandler(),
//		tcpAcceptor(io_service, tcp::endpoint(tcp::v4(), port))
//	{
//		start_accept();
//	}
//
//	TCPServer(boost::asio::io_service& io_service, std::shared_ptr<RequestHandler> requestHandler, uint16_t port = 1337):
//		requestHandler(requestHandler),
//		responseHandler(),
//		tcpAcceptor(io_service, tcp::endpoint(tcp::v4(), port))
//	{
//		start_accept();
//	}
//
//	TCPServer(boost::asio::io_service& io_service, std::shared_ptr<ResponseHandler> responseHandler, uint16_t port = 1337):
//		requestHandler(),
//		responseHandler(responseHandler),
//		tcpAcceptor(io_service, tcp::endpoint(tcp::v4(), port))
//	{
//		start_accept();
//	}
//
//	TCPServer(boost::asio::io_service& io_service, std::shared_ptr<RequestResponseHandler> requestResponseHandler, uint16_t port = 1337):
//		requestHandler(requestResponseHandler),
//		responseHandler(requestResponseHandler),
//		tcpAcceptor(io_service, tcp::endpoint(tcp::v4(), port))
//	{
//		start_accept();
//	}
//
//	virtual ~TCPServer()
//	{
//
//	}
//private:
//	  void start_accept()
//	  {
//		  TCPConnection::pointer new_connection =
//				  TCPConnection::create(tcpAcceptor.get_io_service());
//
//		  tcpAcceptor.async_accept(new_connection->socket(),
//	        boost::bind(&TCPServer::handle_accept, this, new_connection,
//	          boost::asio::placeholders::error));
//
////		  if(ioServiceThread.joinable())
////		  {
////			  ioServiceThread.join();
////		  }
////
//		  std::thread tempThread = std::thread([this](){io_service.run();});
//		  ioServiceThread.swap(tempThread);
//	  }
//
//	  void handle_accept(TCPConnection::pointer new_connection,
//	      const boost::system::error_code& error)
//	  {
//		  std::cout << "NEW CONNECTION" << std::endl;
//	    if (!error)
//	    {
//	      new_connection->start();
//	    }
//
//	    start_accept();
//	  }
//
//		boost::asio::io_service io_service;
//	std::shared_ptr<RequestHandler> requestHandler;
//	std::shared_ptr<ResponseHandler> responseHandler;
//
//
//	tcp::acceptor tcpAcceptor;
//
//	std::thread ioServiceThread;
//};

class TCPServerClient
{
public:
	TCPServerClient(boost::asio::io_service& aIoService,std::string host, std::string remotePort, std::string localPort):
		host(host),
		remotePort(remotePort),
		localPort(localPort),
		acceptor( aIoService, boost::asio::ip::tcp::endpoint( boost::asio::ip::tcp::v4(), std::atoi(localPort.c_str()))),
		ioService(aIoService)
{
		handleAccept( std::make_shared<TCPSession>(TCPSession(ioService)), boost::system::error_code());
	}

	void handleAccept( 	std::shared_ptr<TCPSession> aSession,
						const boost::system::error_code& error)
	{
		std::cout << __PRETTY_FUNCTION__ << std::endl;
		if (!error)
		{
			if(!ioService.stopped())
			{
				std::cout << "We hebben een binnenkomende verbinding" << std::endl;
				std::shared_ptr<TCPSession> session = std::shared_ptr<TCPSession>(new TCPSession( ioService));

				acceptor.async_accept( session->getSocket(),
						boost::bind( &TCPServerClient::handleAccept, this, session, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
				if (aSession)
				{
					std::cout << "We gaan een sessie starten" << std::endl;
					aSession->handleIncommingConnection();
				}
				else
				{
					std::cout << "We hebben geen sessie" << std::endl;
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

	void sendMessage()
	{
		std::cout << __PRETTY_FUNCTION__ << std::endl;
		std::shared_ptr<TCPSession> session = std::shared_ptr<TCPSession>(new TCPSession(ioService));

		boost::asio::ip::tcp::resolver resolver( ioService);
		boost::asio::ip::tcp::resolver::query query( boost::asio::ip::tcp::v4(), host, remotePort);
		boost::asio::ip::tcp::resolver::iterator endpoint_iterator = resolver.resolve( query);
		boost::asio::ip::tcp::endpoint endpoint = *endpoint_iterator;

		session->getSocket().async_connect( endpoint,
											boost::bind( &TCPServerClient::handleOutgoingConnection, this, session, boost::asio::placeholders::error));
	}
	virtual~ TCPServerClient()
	{
		acceptor.cancel();
		acceptor.close();
	}
private:

	void handleConnect( std::shared_ptr<TCPSession> aSession,
						const boost::system::error_code& error,
						std::size_t bytesTransferd)
	{
		if(!error)
		{
			aSession->start();
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
