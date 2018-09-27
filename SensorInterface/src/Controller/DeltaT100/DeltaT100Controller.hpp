/*
 * DeltaT100Controller.hpp
 *
 *  Created on: 13 Jun 2018
 *      Author: Thomas Maters
 *		Email : thomasmaters@hotmail.com (TG.Maters@student.han.nl)
 */

#ifndef SRC_DELTAT100CONTROLLER_HPP_
#define SRC_DELTAT100CONTROLLER_HPP_

#include "../../Communication/ConnectionInterface.hpp"
#include "../../Communication/TCP/TCPConnection.hpp"
#include "../../Communication/UDP/UDPConnection.hpp"
#include "../../DataBuffer.hpp"
#include "../../Messages/ProfilePointOutput.hpp"
#include "../../Messages/SwitchDataCommand.hpp"
#include "../../Profiler/DepthProfiler.hpp"

#define DELTAT100_BEAM_COUNT 480
#define DELTAT100_BUF_SIZE 2048
#define DELTAT100_DEBUG 0

namespace Controller::DeltaT100
{
/**
 * Class responsible for communicating with DeltaT100 Multibeam or DeltaT.exe.
 */
using namespace Messages;
class DeltaT100Controller : public Communication::RequestHandler, public Communication::ResponseHandler
{
  public:
    DeltaT100Controller(const std::string& host, const std::string& remote_port, const std::string& local_port);

    /**
     * Handles a response from the DeltaT100 Multibeam.
     * @param data
     * @param length of response.
     * @param time of request.
     */
    void handleResponse(uint8_t* data, std::size_t length, std::chrono::milliseconds::rep time = 0) override;

    /**
     * Handles a UDP message from DeltaT.exe.
     * @param data
     * @param length of request
     * @param time of request.
     * @return Response message
     */
    SensorMessage handleRequest(uint8_t* data, std::size_t length, std::chrono::milliseconds::rep time = 0) override;

    /**
     * Sends a request for a SonarReturnDataPacket.
     * @param command Switch command.
     */
    void requestSensorPing(const SwitchDataCommand& command);

    virtual ~DeltaT100Controller();

  public:
    /**
     * Constructs a SonarReturnData message from the databuffer.
     */
    void cosntructSensorPing();

    Communication::TCP::TCPServerClient sensor_communication_;
    Communication::UDP::UDPServerClient deltat_communication_;

    SwitchDataCommand switch_data_command_;
    ProfilePointOutput profile_point_output_;

    uint16_t display_gain_;
    uint16_t current_display_gain_;

    Profiler::DepthProfiler<DELTAT100_BEAM_COUNT, float> depth_profiler_;

    std::unique_ptr<DataBuffer<DELTAT100_BUF_SIZE>> data_buffer_;
};

} /* namespace Controller::DeltaT100 */

#endif /* SRC_DELTAT100CONTROLLER_HPP_ */
