/*
 * IOHandler.hpp
 *
 *  Created on: 7 Aug 2018
 *      Author: Thomas Maters
 *		Email : thomasmaters@hotmail.com (TG.Maters@student.han.nl)
 */

#ifndef SRC_IOHANDLER_HPP_
#define SRC_IOHANDLER_HPP_

#include <boost/asio/io_service.hpp>

#include <thread>

class IOHandler
{
  public:
    static IOHandler& getInstance()
    {
        static IOHandler instance;
        return instance;
    }

    boost::asio::io_service& getIOService()
    {
        return io_service_;
    }

    void startIOService()
    {
        io_thread_ = std::thread([this]() { io_service_.run(); });
    }

    void startIOServiceBlocking()
    {
        auto work = std::make_shared<boost::asio::io_service::work>(io_service_);
        io_service_.run();
    }

    void stopIOService()
    {
        std::cout << __PRETTY_FUNCTION__ << std::endl;
        if (!io_service_.stopped())
        {
            io_service_.stop();
        }
    }

    ~IOHandler()
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

  private:
    IOHandler()
    {
    }

    boost::asio::io_service io_service_;
    std::thread io_thread_;
};

#endif /* SRC_IOHANDLER_HPP_ */
