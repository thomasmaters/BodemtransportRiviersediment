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
    	Matrix<1,5,T> inputs({{0,1,2,3,4,5}});
    	Matrix<1,5,T> outputs({{5,6,3,3,4,1}});
    	Matrix<1,4,T> params({{1,1,5,1}});
    	GaussNewton(inputs, outputs, params);
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

    template <std::size_t H>
    Matrix<H - 1, 2, T> calculateAfgeleide(Matrix<H, 3, T>& matrix)
    {
        Matrix<H - 1, 2, T> result;

        // TODO: Do we get the correct matrix values?
        for (std::size_t i = 0; i < N - 1; ++i)
        {
            result[i][0] = matrix[i][0];
            result[i][1] = (matrix[i + 1][1] - matrix[i][1]) / matrix[i + 1][0] - matrix[i][0];
        }
        return result;
    }

    template <std::size_t H>
    std::vector<std::size_t> findPeaksAndValleys(Matrix<H, 3, T>& afgeleide,
                                                 float afweiking,
                                                 std::size_t minimal_x_diff = 0)
    {
        std::vector<std::size_t> result;
        for (std::size_t i = 0; i < H - 1; ++i)
        {
            if (afgeleide[i][1] > -afweiking && afgeleide[i][1] < afweiking &&
                abs(afgeleide[i + 1][0] - afgeleide[i][0]) > minimal_x_diff)
            {
                result.push_back(i);
            }
        }
        return result;
    }

    template <std::size_t H>
    void classifyWave(Matrix<H, 3, T>& matrix, std::vector<std::size_t> peaks_and_valleys)
    {
        std::vector<std::size_t> pieken;
        std::vector<std::size_t> dalen;
        for (std::size_t i = 0; i < peaks_and_valleys.size() - 1; ++i)
        {

        }
    }

    const float DERIV_STEP = 1e-5;
    const std::size_t MAX_ITER = 100;

    template<std::size_t H, std::size_t aH>
    T Func(const Matrix<H,1,T>& input, const Matrix<aH,1,T>& params)
    {

    	T A = params.at(0,0);
    	T B = params.at(1,0);
    	T C = params.at(2,0);
    	T D = params.at(3,0);

    	T x = input.at(0,0);

        return A*sin(B*x) + C*cos(D*x);
    }

    template<std::size_t H, std::size_t aH>
    T Deriv(const Matrix<H,1,T> &input, const Matrix<aH,1,T> &params, std::size_t n)
    {
    	// Assumes input is a single row matrix

    	// Returns the derivative of the nth parameter
    	Matrix<aH,1,T> params1(params);
    	Matrix<aH,1,T> params2(params);

    	// Use central difference  to get derivative
    	params1.at(n,0) -= DERIV_STEP;
    	params2.at(n,0) += DERIV_STEP;

    	T p1 = Func(input, params1);
    	T p2 = Func(input, params2);

    	return (p2 - p1) / (2*DERIV_STEP);
    }

    template<std::size_t H, std::size_t aH>
    void GaussNewton(const Matrix<H,1,T> &inputs, const Matrix<H,1,T> &outputs, Matrix<aH,1,T> &params)
    {
        std::size_t m = inputs.getHeight();
        std::size_t n = inputs.getWidth();
        std::size_t num_params = params.getHeight();

        Matrix<H,1,T> r; // residual matrix
        Matrix<H,aH,T> Jf; // Jacobian of Func()
        Matrix<1,1,T> input; // single row input

        T last_mse = 0;

        for(std::size_t i=0; i < MAX_ITER; i++) {
            T mse = 0;

            for(std::size_t j=0; j < m; j++) {
            	for(std::size_t k=0; k < n; k++) {
            		input.at(0,k) = inputs.at(j,k);
            	}

                r.at(j,0) = outputs.at(j,0) - Func(input, params);

                mse += r.at(j,0)*r.at(j,0);

                for(std::size_t k=0; k < num_params; k++) {
                	Jf.at(j,k) = Deriv(input, params, k);
                }
            }

            mse /= m;

            // The difference in mse is very small, so quit
            if(fabs(mse - last_mse) < 1e-8) {
            	break;
            }

            auto delta = (Jf.transpose() * Jf).inverse() * Jf.transpose() *r;
            params += delta;

            std::cout << "I: " << i << " " << mse << std::endl;

            last_mse = mse;
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
