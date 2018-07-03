/*
 * DepthProfiler.hpp
 *
 *  Created on: 2 Jul 2018
 *      Author: Thomas Maters
 *		Email : thomasmaters@hotmail.com (TG.Maters@student.han.nl)
 */

#ifndef SRC_DEPTHPROFILER_HPP_
#define SRC_DEPTHPROFILER_HPP_

#include "DataBuffer.hpp"
#include "SensorPing.hpp"

#include <thread>
#include <vector>

namespace Controller
{
/*
 *
 */
class DepthProfiler
{
  public:
    static DepthProfiler& getInstance()
    {
        static DepthProfiler instance;
        return instance;
    }

    void addSensorPing(SensorPing ping)
    {
        depth_data_.push_back(ping);
        try
        {
            DeltaT100::DeltaT100Ping& a = dynamic_cast<DeltaT100::DeltaT100Ping&>(ping);
            std::cout << "Writing to output" << std::endl;
            a.toDeltaT837(output_stream_);
        }
        catch (std::exception& e)
        {
            std::cout << "dynamic cast failed: " << e.what() << std::endl;
        }
    }

    void sensorPingsToFile();
    void sensorPingsFromFile();

  private:
    DepthProfiler()
    {
        output_stream_.open("frituur.837", std::ios::app | std::ios::binary);
    }

    virtual ~DepthProfiler()
    {
        output_stream_.close();
    }

  private:
    std::ofstream output_stream_;
    std::vector<SensorPing> depth_data_;
    std::thread profiler_thread_;
};

} /* namespace Controller */

#endif /* SRC_DEPTHPROFILER_HPP_ */
