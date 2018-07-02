/*
 * DepthProfiler.hpp
 *
 *  Created on: 2 Jul 2018
 *      Author: Thomas Maters
 *		Email : thomasmaters@hotmail.com (TG.Maters@student.han.nl)
 */

#ifndef SRC_DEPTHPROFILER_HPP_
#define SRC_DEPTHPROFILER_HPP_

#include "SensorPing.hpp"
#include "DataBuffer.hpp"

#include <vector>
#include <thread>

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
    }

    void sensorPingsToFile();
    void sensorPingsFromFile();

  private:
    DepthProfiler()
  {

  }

    virtual ~DepthProfiler()
    {

    }
  private:
    std::vector<SensorPing> depth_data_;
    std::thread profiler_thread_;
};

} /* namespace Controller */

#endif /* SRC_DEPTHPROFILER_HPP_ */
