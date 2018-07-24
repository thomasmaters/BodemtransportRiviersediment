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
#include "Dune.hpp"
#include "FileHandler.hpp"
#include "Matrix.hpp"

#include <thread>
#include <valarray>
#include <vector>

#define MAX_ITERATIONS 200
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
    }

    virtual ~DepthProfiler()
    {
    }

    void addRawPoint(Matrix<N, 3, T>& matrix)
    {
        // Apply filters and stuff.
        addProcessedPoint(matrix);
    }

    float calculateArea(const Matrix<4, 1, T>& params, T start, T end)
    {
        float y1 = ((1.0 / 4) * params.at(0, 0) * std::pow(start, 4)) +
                   ((1.0 / 3) * params.at(1, 0) * std::pow(start, 3)) +
                   ((1.0 / 2) * params.at(2, 0) * std::pow(start, 2)) + params.at(3, 0) * start;
        float y2 = ((1.0 / 4) * params.at(0, 0) * std::pow(end, 4)) + ((1.0 / 3) * params.at(1, 0) * std::pow(end, 3)) +
                   ((1.0 / 2) * params.at(2, 0) * std::pow(end, 2)) + params.at(3, 0) * end;
        return std::abs(y2 - y1);
    }

    void addProcessedPoint(Matrix<N, 3, T>& matrix)
    {
        try
        {
            BottomProfile<N, 3, T> bottom_profile;
            bottom_profile.raw_data_ = matrix;

            // Calculate ax^3 + bx^2 + cx + d
            auto afgeleide                             = calculateAfgeleide(matrix);
            std::vector<std::size_t> peaks_and_valleys = findPeaksAndValleys(afgeleide, 0.001, 1);
            std::vector<std::pair<std::size_t, std::size_t>> waves = classifyWave(afgeleide, peaks_and_valleys);

            for (std::size_t i = 0; i < waves.size(); ++i)
            {
                Dune dune;
                dune.size_  = waves[i].second - waves[i].first;
                dune.start_ = waves[i].first;

                // TODO: change this magic number.
                if (dune.size_ > 60)  // Our wave has more then 60 points
                {
                    continue;
                }
                if (dune.start_ + 60 > N)  // Our wave doesn't fit in matrix, this is really bad coding.
                {
                    return;
                }
                Matrix<60, 2, float> single_dune_matrix(matrix, dune.size_, 0);

                // Translate x values so they start from 0. (This helps the Gausse-Newton algorithme find a result.)
                T start_x = single_dune_matrix.at(0, 0);
                for (std::size_t i = 0; i < dune.size_ + 1; ++i)
                {
                    single_dune_matrix.at(i, 0) -= start_x;
                }

                Matrix<4, 1, T> params({ 1, 1, 1, 1 });
                gaussNewton(single_dune_matrix, params, dune.size_);
                dune.signature_ = params;
                dune.area_ = calculateArea(params, single_dune_matrix.at(0, 0), single_dune_matrix.at(dune.size_, 0));
                bottom_profile.dunes_.push_back(dune);
                std::cout << dune.toString() << std::endl;
                std::cout << params << std::endl;
            }

            depth_data_.push_back(bottom_profile);
        }
        catch (std::exception& e)
        {
            std::cout << e.what() << std::endl;
        }
    }

    template <std::size_t H, std::size_t W>
    Matrix<H - 1, 2, T> calculateAfgeleide(Matrix<H, W, T>& matrix) const
    {
        Matrix<H - 1, 2, T> result;

        // TODO: Do we get the correct matrix values?
        for (std::size_t i = 0; i < H - 1; ++i)
        {
            result[i][0] = matrix[i][0];
            result[i][1] = (matrix[i + 1][1] - matrix[i][1]) / (matrix[i + 1][0] - matrix[i][0]);
        }
        return result;
    }

    template <std::size_t H, std::size_t W>
    std::vector<std::size_t> findPeaksAndValleys(const Matrix<H, W, T>& afgeleide,
                                                 float afweiking,
                                                 std::size_t minimal_x_diff = 0) const
    {
        std::vector<std::size_t> result;
        float last_found_x = 0;
        for (std::size_t i = 0; i < H - 1; ++i)
        {
            if (((afgeleide[i][1] < 0 && afgeleide[i + 1][1] > 0) ||
                 (afgeleide[i][1] > 0 && afgeleide[i + 1][1] < 0)) &&
                abs(afgeleide[i][0] - last_found_x) > minimal_x_diff)
            {
                last_found_x = afgeleide[i][0];
                result.push_back(i);
            }
        }
        std::cout << "Found: " << result.size() << " peaks or valleys" << std::endl;
        return result;
    }

    template <std::size_t H, std::size_t W>
    bool isPeak(const Matrix<H, W, T>& afgeleide, std::size_t index) const
    {
        assert(index + 2 < H);
        return afgeleide.at(index, 1) > afgeleide.at(index + 1, 1) &&
               afgeleide.at(index + 1, 1) > afgeleide.at(index + 2, 1);
    }

    template <std::size_t H, std::size_t W>
    std::vector<std::pair<std::size_t, std::size_t>>
    classifyWave(const Matrix<H, W, T>& afgeleide, const std::vector<std::size_t>& peaks_and_valleys) const
    {
        std::vector<std::pair<std::size_t, std::size_t>> result;
        for (std::size_t i = 0; i < peaks_and_valleys.size() - 3; ++i)
        {
            // Find a valley -> peak -> valley order.
            if (!isPeak(afgeleide, peaks_and_valleys[i]) && isPeak(afgeleide, peaks_and_valleys[i + 1]) &&
                !isPeak(afgeleide, peaks_and_valleys[i + 2]))
            {
                result.push_back(std::make_pair(peaks_and_valleys[i], peaks_and_valleys[i + 2]));
                std::cout << "Found a wave from:" << peaks_and_valleys[i] << " to: " << peaks_and_valleys[i + 2]
                          << std::endl;
            }
        }
        return result;
    }

    /**
     * Function that the GaussNewton tries to calculate the parameters for.
     */
    template <std::size_t H, std::size_t aH>
    T baseFunction(const Matrix<H, 1, T>& input, const Matrix<aH, 1, T>& params) const
    {
        T x = input.at(0, 0);

        return params.at(0, 0) * std::pow(x, 3) + params.at(1, 0) * std::pow(x, 2) + params.at(2, 0) * x +
               params.at(3, 0);
    }

    /**
     * Calculated derivative using: (y1 - y0)/(x1-x0).
     * But when doing so, it will change one parameter with a slight amount (DERIV_STEP).
     */
    template <std::size_t H, std::size_t aH>
    T calculateDerivative(const Matrix<H, 1, T>& input, const Matrix<aH, 1, T>& params, std::size_t param_number) const
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
    void gaussNewton(const Matrix<H, W, T>& inputs,
                     Matrix<aH, 1, T>& params,
                     std::size_t iterations = H,
                     const float precision  = 1e-8) const
    {
        assert(H >= W);

        Matrix<H, 1, T> residuals;  // Residual matrix
        Matrix<H, aH, T> jacobian;  // Jacobian matrix
        Matrix<1, 1, T> input;      // Single row input

        float last_deviation = 0;

        for (std::size_t i = 0; i < MAX_ITERATIONS; ++i)
        {
            float total_deviation = 0;

            for (std::size_t j = 0; j < iterations; ++j)
            {
                input.at(0, 0) = inputs.at(j, 0);

                // r_i(beta)= y_i - f(x_i, \beta).
                residuals.at(j, 0) = inputs.at(j, 1) - baseFunction(input, params);
                // Add the square difference of the last derived parameter and the wanted value to the total deviation.
                total_deviation += residuals.at(j, 0) * residuals.at(j, 0);

                for (std::size_t k = 0; k < aH; ++k)
                {
                    jacobian.at(j, k) = calculateDerivative(input, params, k);
                }
            }

            // Mean deviation over all the inputs.
            total_deviation /= H;

            // TODO: Maybe change this check to only check for the current deviation |total_deviation| < precision
            // If the deviation is since last iteration is smaller then precision, we have found the best optimal value.
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
    std::vector<BottomProfile<N, 3, T>> depth_data_;
};

} /* namespace Controller */

#endif /* SRC_DEPTHPROFILER_HPP_ */
