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

namespace Communication
{
class IOHandler
{
  public:
    /**
     * Get reference to instance.
     * @return
     */
    static IOHandler& getInstance()
    {
        static IOHandler instance;
        return instance;
    }

    /**
     * Get a reference to the io_service.
     * @return
     */
    boost::asio::io_service& getIOService();

    /**
     * Starts the io_service in a separate thread.
     */
    void startIOService();

    /**
     * Starts the io_service in the main thread.
     */
    void startIOServiceBlocking();

    /**
     * Stops the io_service and kills any running thread.
     */
    void stopIOService();

    ~IOHandler();

  private:
    IOHandler()
    {
    }

    boost::asio::io_service io_service_;
    std::thread io_thread_;
};
}  // Namespace Communication

#endif /* SRC_IOHANDLER_HPP_ */
