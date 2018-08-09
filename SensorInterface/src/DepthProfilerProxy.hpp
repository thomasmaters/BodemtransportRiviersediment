/*
 * DepthProfilerProxy.hpp
 *
 *  Created on: 6 Aug 2018
 *      Author: Thomas Maters
 *		Email : thomasmaters@hotmail.com (TG.Maters@student.han.nl)
 */

#ifndef SRC_DEPTHPROFILERPROXY_HPP_
#define SRC_DEPTHPROFILERPROXY_HPP_

#include "BottomTransportMessage.hpp"
#include "Dune.hpp"
#include "IOHandler.hpp"
#include "UDPConnection.hpp"

#include <thread>

class DepthProfilerProxy : public Communication::ResponseHandler,
                           public std::enable_shared_from_this<DepthProfilerProxy>
{
  public:
    DepthProfilerProxy() : outgoing_communication_(IOHandler::getInstance().getIOService(), "localhost", "2000", "2001")
    {
        std::cout << __PRETTY_FUNCTION__ << std::endl;
        outgoing_communication_.addResponseHandler(std::shared_ptr<ResponseHandler>(this));
    }

    ~DepthProfilerProxy()
    {
        std::cout << __PRETTY_FUNCTION__ << std::endl;
    }

    void handleResponse([[maybe_unused]] uint8_t* data, [[maybe_unused]] std::size_t length)
    {
        std::cout << __PRETTY_FUNCTION__ << std::endl;
    }

    template <std::size_t H, std::size_t W, typename T>
    void sendBottomProfile(BottomProfile<H, W, T>& profile)
    {
        std::cout << __PRETTY_FUNCTION__ << std::endl;
        Controller::BottomTransportMessage message;
        message.setDunes(profile);
        message.setAverageTransport(profile.average_transport_);
        message.setTimeOfPing(profile.time_);
        outgoing_communication_.sendRequest(message, (std::size_t)0, false);
    }

  private:
    Communication::UDP::UDPServerClient outgoing_communication_;
};

#endif /* SRC_DEPTHPROFILERPROXY_HPP_ */
