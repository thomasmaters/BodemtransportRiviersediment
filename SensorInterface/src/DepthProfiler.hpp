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

#define MAX_ITERATIONS 100
#define STEP_SIZE 1e-5

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
    DepthProfiler()
    {
    	try {
            Matrix<31, 1, T> inputs({ 0,      0.1695, 0.376,  0.6165, 0.888,  1.1875, 1.512,  1.8585, 2.224,  2.6055, 3,
                                      3.4235, 3.888,  4.3845, 4.904,  5.4375, 5.976,  6.5105, 7.032,  7.5315, 8,      8.445,
                                      8.88,   9.305,  9.72,   10.125, 10.52,  10.905, 11.28,  11.645, 12 });
            Matrix<31, 1, T> outputs({ 4,     4.03875, 4.1,   4.17625, 4.26,  4.34375, 4.42,  4.48125,
                                       4.52,  4.52875, 4.5,   4.41975, 4.288, 4.11825, 3.924, 3.71875,
                                       3.516, 3.32925, 3.172, 3.05775, 3,     3.001,   3.048, 3.132,
                                       3.244, 3.375,   3.516, 3.658,   3.792, 3.909,   4 });
            Matrix<4, 1, T> params({ 1, 1, 1, 1 });
            gaussNewton(inputs, outputs, params);
            std::cout << params << std::endl;
		} catch (std::exception& e) {
			std::cout << e.what() << std::endl;
		}

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

    /**
     * Function that the GaussNewton tries to calculate the parameters for.
     */
    template <std::size_t H, std::size_t aH>
    T baseFunction(const Matrix<H, 1, T>& input, const Matrix<aH, 1, T>& params)
    {
        T x = input.at(0, 0);

        return params.at(0, 0) * std::pow(x, 3) + params.at(1, 0) * std::pow(x, 2) + params.at(2, 0) * x +
               params.at(3, 0);
    }

    /**
     * Calculated derivative using: (y1 - y0)/(x1-x0).
     * But when doing so, it will change a parameter with a slight amount (DERIV_STEP).
     */
    template <std::size_t H, std::size_t aH>
    T calculateDerivative(const Matrix<H, 1, T>& input, const Matrix<aH, 1, T>& params, std::size_t param_number)
    {
        // Make copy of the current parameters.
        Matrix<aH, 1, T> params1(params);
        Matrix<aH, 1, T> params2(params);

        // Use central difference  to get derivative.
        params1.at(param_number, 0) -= STEP_SIZE;
        params2.at(param_number, 0) += STEP_SIZE;

        T p1 = baseFunction(input, params1);
        T p2 = baseFunction(input, params2);

        return (p2 - p1) / (2 * STEP_SIZE);
    }

    /**
     * Solve the non-linear least square problem using the Gauss-Newton algorithme.
     * https://en.wikipedia.org/wiki/Gauss%E2%80%93Newton_algorithm
     */
    template <std::size_t H, std::size_t W, std::size_t aH>
    void gaussNewton(const Matrix<H, W, T>& inputs, const Matrix<H, W, T>& outputs, Matrix<aH, 1, T>& params, const float precision = 1e-8)
    {
        assert(H >= W);

        Matrix<H, 1, T> residuals;  // Residual matrix
        Matrix<H, aH, T> jacobian;  // Jacobian matrix
        Matrix<1, 1, T> input;      // Single row input

        float last_deviation = 0;

        for (std::size_t i = 0; i < MAX_ITERATIONS; i++)
        {
            float total_deviation = 0;

            for (std::size_t j = 0; j < H; j++)
            {
            	//Some compile time optimalisations.
                if
                    constexpr(W == 1)
                    {
                        input.at(0, 0) = inputs.at(j, 0);
                    }
                else
                {
                    for (std::size_t k = 0; k < W; k++)
                    {
                        input.at(0, k) = inputs.at(j, k);
                    }
                }

                // r_i(beta)= y_i - f(x_i, \beta).
                residuals.at(j, 0) = outputs.at(j, 0) - baseFunction(input, params);
                // Add the square difference of the last derived parameter and the wanted value to the total deviation.
                total_deviation += residuals.at(j, 0) * residuals.at(j, 0);

                for (std::size_t k = 0; k < aH; k++)
                {
                    jacobian.at(j, k) = calculateDerivative(input, params, k);
                }
            }

            // Mean deviation over all the inputs.
            total_deviation /= H;

            //TODO: Maybe change this check to only check for the current deviation |total_deviation| < precision
            //If the deviation is since last iteration is smaller then precision, we have found the best optimal value.
            if (std::abs(total_deviation - last_deviation) < precision)
            {
                break;
            }

            // beta^(s+1) = beta^s - (Jf.T * Jf)^-1 * Jf.T * r(beta^s)
            auto jacobijnTranspose = jacobian.transpose();
            auto delta             = (jacobijnTranspose * jacobian).inverse() * jacobijnTranspose * residuals;
            last_deviation         = total_deviation;

            params += delta;

            if (std::isnan(total_deviation))
            {
                throw std::runtime_error("NaN is found");
            }
            if (i + 1 == MAX_ITERATIONS)
            {
                throw std::runtime_error("Took to many iterations");
            }

            std::cout << "Iteration: " << i << " " << total_deviation << std::endl;
        }
    }

  private:
    FileHandler file_instance_;
    std::vector<Matrix<N, 3, T>> raw_depth_data_;
    std::vector<Matrix<N, 3, T>> depth_data_;
};

} /* namespace Controller */

#endif /* SRC_DEPTHPROFILER_HPP_ */
