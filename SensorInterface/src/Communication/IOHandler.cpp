/*
 * IOHandler.cpp
 *
 *  Created on: 24 Sep 2018
 *      Author: Thomas Maters
 *		Email : thomasmaters@hotmail.com (TG.Maters@student.han.nl)
 */

#include "IOHandler.hpp"

namespace Communication
{

static IOHandler& IOHandler::getInstance()
{
	static IOHandler instance;
	return instance;
}

boost::asio::io_service& IOHandler::getIOService()
{
	return io_service_;
}

void IOHandler::startIOService()
{
	if(!io_thread_.joinable())
	{
		io_thread_ = std::thread([this]() { io_service_.run(); });
	}
}

void IOHandler::startIOServiceBlocking()
{
	auto work = std::make_shared<boost::asio::io_service::work>(io_service_);
	io_service_.run();
}

void IOHandler::stopIOService()
{
	if (!io_service_.stopped())
	{
		io_service_.stop();
	}
	if (io_thread_.joinable())
	{
		io_thread_.join();
	}
}

IOHandler::~IOHandler()
{
	stopIOService();
}
}


