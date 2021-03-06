/*
 * DepthProfilerProxy.hpp
 *
 *  Created on: 6 Aug 2018
 *      Author: Thomas Maters
 *		Email : thomasmaters@hotmail.com (TG.Maters@student.han.nl)
 */

#ifndef SRC_DEPTHPROFILERPROXY_HPP_
#define SRC_DEPTHPROFILERPROXY_HPP_

#include "../Communication/IOHandler.hpp"
#include "../Communication/RequestResponseHandler.hpp"
#include "../Communication/UDP/UDPConnection.hpp"
#include "../Messages/BottomTransportMessage.hpp"
#include "BottomProfile.hpp"
#include "Dune.hpp"

#include <thread>
namespace Profiler
{
class DepthProfilerProxy : public Communication::ResponseHandler,
                           public std::enable_shared_from_this<DepthProfilerProxy>
{
  public:
    DepthProfilerProxy()
      : self_ptr_(weak_from_this()),
        outgoing_communication_(Communication::IOHandler::getInstance().getIOService(), "localhost", "2000", "2001")

    {
        outgoing_communication_.addResponseHandler(self_ptr_.lock());
    }

    virtual ~DepthProfilerProxy()
    {
    }

    /**
     * Handles incoming response messages, override this function to do things with the response.
     * @param data Pointer to the data.
     * @param length Of the data.
     */
    void handleResponse([[maybe_unused]] uint8_t* data, [[maybe_unused]] std::size_t length)
    {
    }

    /**
     * Sends a bottomprofile over a UDP connection as a BottomTransportMessage.
     * @param profile To send.
     */
    template <std::size_t H, std::size_t W, typename T>
    void sendBottomProfile(BottomProfile<H, W, T>& profile)
    {
        Messages::BottomTransportMessage message;
        message.setDunes(profile);
        message.setAverageTransport(profile.average_transport_);
        message.setTimeOfPing(profile.time_);
        outgoing_communication_.sendRequest(message, (std::size_t)0, false);
    }

  private:
    std::weak_ptr<DepthProfilerProxy> self_ptr_;
    Communication::UDP::UDPServerClient outgoing_communication_;
};
}  // Namespace Profiler

#endif /* SRC_DEPTHPROFILERPROXY_HPP_ */
