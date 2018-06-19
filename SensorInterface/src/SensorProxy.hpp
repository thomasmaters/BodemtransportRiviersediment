/*
 * SensorProxy.hpp
 *
 *  Created on: 12 Jun 2018
 *      Author: Thomas Maters
 *		Email : thomasmaters@hotmail.com (TG.Maters@student.han.nl)
 */

#ifndef SRC_SENSORPROXY_HPP_
#define SRC_SENSORPROXY_HPP_

#include "boost/asio.hpp"
#include <iostream>

namespace Controller
{

/*
 *
 */
class SensorProxy
{
public:
	static SensorProxy& getInstance()
	{
		static SensorProxy sensorProxy;
		return sensorProxy;
	}

	void do_receive()
	{
		std::cout << __PRETTY_FUNCTION__ << std::endl;
		socket_.async_receive_from(
		boost::asio::buffer(data_, 1024), sender_endpoint_,
		[this](boost::system::error_code ec, std::size_t bytes_recvd)
			{
				if (!ec && bytes_recvd > 0)
				{
					std::cout << "Received: " << data_ << std::endl;
					do_send(bytes_recvd);
				}
				else
				{
					std::cout << "Nothing received" << std::endl;
					do_receive();
				}
			}
		);
	}

	void do_send(std::size_t length)
	{
		std::cout << __PRETTY_FUNCTION__ << std::endl;
		socket_.async_send_to(
		boost::asio::buffer(data_, length), sender_endpoint_,
		[this](boost::system::error_code ec, std::size_t bytes_sent)
			{
				if(ec)
				{
					std::cout << ec.value() << " reason: " << ec.message() << " bytes send: " << bytes_sent << std::endl;
				}
				std::cout << "Send " << bytes_sent << " bytes" << std::endl;
				do_receive();
			}
		);
	}

	virtual ~SensorProxy()
	{

	}
private:
	SensorProxy(): io_service_(),socket_(io_service_, boost::asio::ip::udp::endpoint(boost::asio::ip::udp::v4(), 1337))
	{
		boost::asio::ip::udp::resolver resolver(io_service_);
		boost::asio::ip::udp::resolver::query query(boost::asio::ip::udp::v4(), "localhost", "1337");
		boost::asio::ip::udp::resolver::iterator iter = resolver.resolve(query);
		sender_endpoint_ = *iter;

//		do_receive();
		do_send(10);

		io_service_thread_.join();
	}

	std::thread io_service_thread_;

	boost::asio::io_service io_service_;
	boost::asio::ip::udp::socket socket_;
	boost::asio::ip::udp::endpoint sender_endpoint_;
	char data_[10];
};

} /* namespace Controller */

#endif /* SRC_SENSORPROXY_HPP_ */
