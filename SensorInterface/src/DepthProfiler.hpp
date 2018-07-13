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
#include "FileHandler.hpp"
#include "Matrix.hpp"

#include <thread>
#include <vector>

namespace Controller
{
/*
 *
 */
// Datapoints per ping. Type to store the pings as.
template <std::size_t N, typename T = float>
class DepthProfiler
{
    static_assert(std::is_arithmetic<T>::value, "Type must be aritmatic.");

  public:
    DepthProfiler() : gain(1)
    {
    }

    virtual ~DepthProfiler()
    {
    }

    void addRawPoint(Matrix<N, 3, T> matrix)
    {
        raw_depth_data_.push_back(matrix);
    }

    void addProcessedPoint(Matrix<N, 3, T> matrix)
    {
        std::cout << matrix << std::endl;
        depth_data_.push_back(matrix);
    }

    Matrix<N, 3, T> getPointsWithDisplayGain(std::size_t index, uint16_t gain)
    {
        if (gain == 0)
        {
            return raw_depth_data_.at(index);
        }
        if (raw_depth_data_.size() <= index)
        {
            return raw_depth_data_.back();
        }
        if (raw_depth_data_.size() > index + gain)
        {
            gain = raw_depth_data_.size() - index;
        }

        Matrix<N, 3, T> temp = raw_depth_data_[index];
        // C = N + N * (gain - 1) worst
        // C = N best
        for (std::size_t i = 0; i < N; ++i)
        {
            if (temp.at(i, 0) == 0)
            {
                for (std::size_t k = 1; k < gain; ++i)
                {
                    if (raw_depth_data_[index + k].at(i, 0) != 0)
                    {
                        temp.at(i, 0) = raw_depth_data_[index + k].at(i, 0);
                        break;
                    }
                }
            }
        }

        return temp;
    }

    void getPointsWithDisplayGain(Matrix<N, 3, T>& matrix, std::size_t remove_index, std::size_t add_index)
    {
        if (remove_index >= raw_depth_data_.size() || add_index >= raw_depth_data_.size())
        {
            throw std::runtime_error("Index out of bounds.");
        }
        for (std::size_t i = 0; i < N; ++i)
        {
            if (raw_depth_data_[add_index].at(i, 0) != 0)
            {
                matrix.at(i, 0) = raw_depth_data_.at(remove_index).at(i, 0);
            }
            else if (raw_depth_data_[remove_index].at(i, 0) != 0)
            {
                for (std::size_t k = remove_index + 1; k < add_index; ++i)
                {
                    if (raw_depth_data_[k].at(i, 0) != 0)
                    {
                        matrix.at(i, 0) = raw_depth_data_[k].at(i, 0);
                        break;
                    }
                }
            }
        }
    }

  private:
    FileHandler file_instance_;
    std::vector<Matrix<N, 3, T>> raw_depth_data_;
    std::vector<Matrix<N, 3, T>> depth_data_;

    std::size_t gain;
};

} /* namespace Controller */

#endif /* SRC_DEPTHPROFILER_HPP_ */
