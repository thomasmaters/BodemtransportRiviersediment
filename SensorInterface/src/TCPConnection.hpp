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

class TCPConnection : public std::enable_shared_from_this<TCPConnection>
{
public:
	typedef std::shared_ptr<TCPConnection> pointer;

	static pointer create(boost::asio::io_service& io_service)
	{
		return pointer(new TCPConnection(io_service));
	}

	tcp::socket& socket()
	{
	return socket_;
	}

	void start()
	{
		message_ = "HelloWorld";

		boost::asio::async_write(socket_, boost::asio::buffer(message_),
		boost::bind(&TCPConnection::handle_write, shared_from_this()));
	}

private:
	TCPConnection(boost::asio::io_service& io_service)
		: socket_(io_service)
	{
	}

	void handle_write()
	{
	}

	tcp::socket socket_;
	std::string message_;
};


class TCPServer
{
public:
	TCPServer(boost::asio::io_service& io_service):
		requestHandler(),
		responseHandler(),
		tcpAcceptor(io_service, tcp::endpoint(tcp::v4(), 13))
	{

	}

	TCPServer(boost::asio::io_service& io_service, std::shared_ptr<RequestHandler> requestHandler):
		requestHandler(requestHandler),
		responseHandler(),
		tcpAcceptor(io_service, tcp::endpoint(tcp::v4(), 13))
	{

	}

	TCPServer(boost::asio::io_service& io_service, std::shared_ptr<ResponseHandler> responseHandler):
		requestHandler(),
		responseHandler(responseHandler),
		tcpAcceptor(io_service, tcp::endpoint(tcp::v4(), 13))
	{

	}

	TCPServer(boost::asio::io_service& io_service, std::shared_ptr<RequestResponseHandler> requestResponseHandler):
		requestHandler(requestResponseHandler),
		responseHandler(requestResponseHandler),
		tcpAcceptor(io_service, tcp::endpoint(tcp::v4(), 13))
	{

	}

	virtual ~TCPServer()
	{

	}
private:
	  void start_accept()
	  {
		  TCPConnection::pointer new_connection =
				  TCPConnection::create(tcpAcceptor.get_io_service());

		  tcpAcceptor.async_accept(new_connection->socket(),
	        boost::bind(&TCPServer::handle_accept, this, new_connection,
	          boost::asio::placeholders::error));
	  }

	  void handle_accept(TCPConnection::pointer new_connection,
	      const boost::system::error_code& error)
	  {
	    if (!error)
	    {
	      new_connection->start();
	    }

	    start_accept();
	  }

	std::shared_ptr<RequestHandler> requestHandler;
	std::shared_ptr<ResponseHandler> responseHandler;

	tcp::acceptor tcpAcceptor;
};

class chat_handler
: public std::enable_shared_from_this<chat_handler>
{
public:
chat_handler(boost::asio::io_service& service)
: service_(service)
, socket_(service)
, write_strand_(service)
{}
boost::asio::ip::tcp::socket& socket()
{
return socket_;
}
void start()
{
	std::cout << "Wij willen gaan lezen" << std::endl;
//	read_packet();
}
private:
	boost::asio::io_service& service_;
	boost::asio::ip::tcp::socket socket_;
	boost::asio::io_service::strand write_strand_;
	boost::asio::streambuf in_packet_;
};

//Connectionhandler is the type of class who i gonna handle when a new connection gets accepted from the server.
template<typename ConnectionHandler = chat_handler>
class TCPServer2
{
public:
	using handlerPointer = std::shared_ptr<ConnectionHandler>;

	TCPServer2(): acceptor_(io_service_)
	{

	}

	void startServer(uint16_t port)
	{
		std::cout << "Starting a server" << std::endl;
		auto handler = std::make_shared<ConnectionHandler>(io_service_);

		//Listen voor shit on the port
		tcp::endpoint endpoint(tcp::v4(), port);
		acceptor_.open(endpoint.protocol());
		acceptor_.set_option(tcp::acceptor::reuse_address(true));
		acceptor_.bind(endpoint);
		acceptor_.listen();
		acceptor_.async_accept(handler->socket(),
			[=](auto ec)
			{
			std::cout << "Lambda" << std::endl;
				handleNewConnection(handler,ec);
			}
		);

		//Do we get a new connection, try to kill the old one.
		if(handlerThread.joinable())
		{
			handlerThread.join();
		}
		std::thread temp = std::thread([this](){io_service_.run();});
		handlerThread.swap(temp);
	}

	virtual ~TCPServer2()
	{

	}

private:
	void handleNewConnection(handlerPointer handler, boost::system::error_code const& error)
	{
		if(error)
		{
			std::cout << "PANIEK, Error: " << error.message() << std::endl;
			if(handlerThread.joinable())
			{
				handlerThread.join();
			}
			return; //Fack deze shit we mogen de client niet.
		}

		handler->start();

		auto newHandler = std::make_shared<ConnectionHandler>(io_service_);
		acceptor_.async_accept(newHandler->socket(), [this, newHandler](auto ec){ handleNewConnection(newHandler,ec); });
	}

	tcp::acceptor acceptor_;
	boost::asio::io_service io_service_;

	//Maybe make a threadpool if multiple clients can connect with the server.
	std::thread handlerThread;
};

}


#endif /* SRC_TCPCONNECTION_HPP_ */
