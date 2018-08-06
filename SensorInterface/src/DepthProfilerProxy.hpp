/*
 * DepthProfilerProxy.hpp
 *
 *  Created on: 6 Aug 2018
 *      Author: Thomas Maters
 *		Email : thomasmaters@hotmail.com (TG.Maters@student.han.nl)
 */

#ifndef SRC_DEPTHPROFILERPROXY_HPP_
#define SRC_DEPTHPROFILERPROXY_HPP_

#include "UDPConnection.hpp"
#include "BottomTransportMessage.hpp"
#include "Dune.hpp"

#include <thread>

class DepthProfilerProxy: public Communication::ResponseHandler
{
public:
	DepthProfilerProxy():
		outgoing_communication_(io_service_,"localhost", "0", "666")
	{
		std::cout << __PRETTY_FUNCTION__ << std::endl;
//		std::thread temp{[this](){io_service_.run();}};
//		temp.detach();
	}

	~DepthProfilerProxy()
	{
		std::cout << __PRETTY_FUNCTION__ << std::endl;
		io_service_.stop();
	}

	void handleResponse([[maybe_unused]] uint8_t* data,[[maybe_unused]] std::size_t length)
	{

	}

	template<std::size_t H, std::size_t W, typename T>
	void sendBottomProfile(BottomProfile<H,W,T>& profile)
	{
		std::cout << __PRETTY_FUNCTION__ << std::endl;
		Controller::BottomTransportMessage message;
		message.setDunes(profile);
		outgoing_communication_.sendRequest(message, (std::size_t)1, false);
	}

private:
	Communication::UDP::UDPServerClient outgoing_communication_;
	boost::asio::io_service io_service_;

};



#endif /* SRC_DEPTHPROFILERPROXY_HPP_ */
