/*
 * DepthProfiler.cpp
 *
 *  Created on: 2 Jul 2018
 *      Author: Thomas Maters
 *		Email : thomasmaters@hotmail.com (TG.Maters@student.han.nl)
 */
#include "../Filter.hpp"

namespace Profiler
{
template <std::size_t N, typename T>
DepthProfiler<N, T>::DepthProfiler(bool send_transport_updates)
  : DepthProfilerProxy(), send_transport_updates_(send_transport_updates)
{
}

template <std::size_t N, typename T>
DepthProfiler<N, T>::~DepthProfiler()
{
}

template <std::size_t N, typename T>
void DepthProfiler<N, T>::addRawPoint(Matrix<N, 3, T>& matrix, std::chrono::milliseconds::rep time)
{
    // Apply filters and stuff.
    addProcessedPoint(matrix, time);
}

template <std::size_t N, typename T>
void DepthProfiler<N, T>::addProcessedPoint(Matrix<N, 3, T>& matrix, std::chrono::milliseconds::rep time)
{
    try
    {
        BottomProfile<N, 3, T> bottom_profile;
        bottom_profile.raw_data_          = matrix;
        bottom_profile.average_transport_ = 0;
        bottom_profile.time_              = time;

        // Calculate ax^3 + bx^2 + cx + d
        auto derivative = calculateDerivative(matrix);
        SmoothFilter::applyFilter(derivative, 1, 5);
        std::vector<std::size_t> peaks_and_valleys = findPeaksAndValleys(matrix, derivative, 5);
        std::vector<std::pair<std::size_t, std::size_t>> waves = classifyWave(derivative, peaks_and_valleys);

        for (std::size_t i = 0; i < waves.size(); ++i)
        {
            Dune dune;
            dune.size_index_  = waves[i].second - waves[i].first;
            dune.start_index_ = waves[i].first;
            dune.start_x_     = matrix.at(dune.start_index_, 0);
            dune.end_x_       = matrix.at(waves[i].second, 0);

            if (dune.size_index_ > DP_MAX_DUNE_POINTS)  // We have a big wave, just skip it. .
            {
#if DEPTHPROFILER_DEBUG > 0
                std::cout << "DepthProfiler: Dune is to big: " << dune.size_index_ << std::endl;
#endif
                continue;
            }
            if (dune.start_index_ + DP_MAX_DUNE_POINTS >
                N)  // Our wave doesn't fit in matrix, this is really bad coding.
            {
#if DEPTHPROFILER_DEBUG > 0
                std::cout << "DepthProfiler: Dune is to big to fit in measurement." << std::endl;
#endif
                continue;
            }
            Matrix<DP_MAX_DUNE_POINTS, 2, float> single_dune_matrix(matrix, dune.start_index_, 0, dune.size_index_);
            // Translate x values so they start from 0. (This helps the Gauss-Newton algorithm find a result.)
            shiftWaveToZero(single_dune_matrix, dune.size_index_);
            try
            {
                Matrix<4, 1, T> params({ 1, 1, 1, single_dune_matrix.at(0, 1) });
                gaussNewton(single_dune_matrix, params, dune.size_index_);
                dune.signature_    = params;
                dune.surface_area_ = calculateArea(params, 0, single_dune_matrix.at(dune.size_index_ - 1, 0));
                bottom_profile.dunes_.push_back(dune);
            }
            catch (std::exception& e)
            {
#if DEPTHPROFILER_DEBUG > 0
                std::cout << "DepthProfiler: Add processed point error: " << e.what() << std::endl;
#endif
            }
        }

        // Calculate the transport!
        if (depth_data_.size() > 0)
        {
            getTransportOverTime(depth_data_.back(), bottom_profile);
            if (send_transport_updates_ && !std::isnan(bottom_profile.average_transport_) &&
                bottom_profile.average_transport_ != 0)
            {
                sendBottomProfile(bottom_profile);
            }
        }

        depth_data_.push_back(bottom_profile);
    }
    catch (std::exception& e)
    {
#if DEPTHPROFILER_DEBUG > 0
        std::cout << "DepthProfiler: Add processed point error: " << e.what() << std::endl;
#endif
    }
}

template <std::size_t N, typename T>
template <std::size_t H>
T DepthProfiler<N, T>::DepthProfiler::newtonsMethod(const Matrix<H, 1, T>& params,
                                                    T inital_guess,
                                                    T x_shift,
                                                    float precision) const
{
    T guess     = inital_guess;
    T new_guess = 0;

    for (std::size_t i = 0; i < MAX_ITERATIONS; ++i)
    {
        T y  = baseFunction(guess, params, x_shift);
        T yp = baseDerivative(guess, params, x_shift);

        if (std::abs(yp) < EPSILON)
        {
            throw std::runtime_error("Newtons Method: Determinat ~= 0");
        }

        new_guess = guess - y / yp;
        if (std::abs(new_guess - guess) <= precision * abs(new_guess))
        {
            return new_guess;
        }

        guess = new_guess;
    }
    throw std::runtime_error("Newtons Method: Took to many iterations.");
}

template <std::size_t N, typename T>
template <std::size_t H>
T DepthProfiler<N, T>::newtonsMethod(const Matrix<H, 1, T>& params,
                                     const Matrix<H, 1, T>& params2,
                                     const std::pair<float, float>& solution_space,
                                     T inital_guess,
                                     T x_shift,
                                     float precision) const
{
#if DEPTHPROFILER_DEBUG > 1
    std::cout << "Newtons method: [" << solution_space.first << "," << solution_space.second
              << "], init: " << inital_guess << " xshift: " << x_shift << std::endl;
#endif
    T guess     = inital_guess;
    T new_guess = 0;

    for (std::size_t i = 0; i < MAX_ITERATIONS; ++i)
    {
        // Keeps in considuration that the second function is shifted along the x axis.
        // Only downside is that it increases the complexity.
        T y  = baseFunction(guess, params) - baseFunction(guess + x_shift, params2);
        T yp = baseDerivative(guess, params) - baseDerivative(guess + x_shift, params2);

        if (std::abs(yp) < EPSILON)
        {
            throw std::runtime_error("Newtons Method: Determinat ~= 0");
        }

        new_guess = guess - y / yp;
#if DEPTHPROFILER_DEBUG > 1
        std::cout << "Newtons method: Iteration: " << i << " guess: " << new_guess << std::endl;
#endif
        if (new_guess < solution_space.first || new_guess > solution_space.second)
        {
            new_guess = inital_guess * std::pow(1.02, i);
            continue;
        }
        else if (std::abs(new_guess - guess) <= precision * abs(new_guess))
        {
            return new_guess;
        }

        guess = new_guess;
    }
    throw std::runtime_error("Newtons Method: Took to many iterations.");
}

template <std::size_t N, typename T>
template <std::size_t H, std::size_t W, std::size_t aH>
void DepthProfiler<N, T>::gaussNewton(const Matrix<H, W, T>& inputs,
                                      Matrix<aH, 1, T>& params,
                                      std::size_t iterations,
                                      const float precision) const
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
        total_deviation /= iterations;

// If the deviation is since last iteration is smaller then precision, we have found the best optimal value.
#if DEPTHPROFILER_DEBUG > 1
        std::cout << "GaussNewtons: Iteration: " << i << " guess: " << total_deviation << std::endl;
#endif
        if (std::abs(total_deviation - last_deviation) < precision)
        {
            break;
        }

        // beta^(s+1) = beta^s - (Jf.T * Jf)^-1 * Jf.T * r(beta^s)
        auto jacobianTranspose = jacobian.transpose();
        auto delta             = (jacobianTranspose * jacobian).inverse() * jacobianTranspose * residuals;
        last_deviation         = total_deviation;

        params += delta;

        if (std::isnan(total_deviation))
        {
            throw std::runtime_error("GaussNewton: NaN is found");
        }
        if (i + 1 == MAX_ITERATIONS)
        {
            throw std::runtime_error("GaussNewton: Took to many iterations");
        }
    }
}

template <std::size_t N, typename T>
template <std::size_t H, std::size_t aH>
T DepthProfiler<N, T>::baseFunction(const Matrix<H, 1, T>& input, const Matrix<aH, 1, T>& params) const
{
    T x = input.at(0, 0);

    return baseFunction(x, params);
}

template <std::size_t N, typename T>
template <std::size_t H>
T DepthProfiler<N, T>::baseFunction(T x, const Matrix<H, 1, T>& params, T h) const
{
    if (h == 0)
    {
        return params.at(0, 0) * std::pow(x, 3) + params.at(1, 0) * std::pow(x, 2) + params.at(2, 0) * x +
               params.at(3, 0);
    }
    else
    {
        T a = params.at(0, 0);
        T b = params.at(1, 0);
        T c = params.at(2, 0);
        T d = params.at(3, 0);

        return (-a) * std::pow(h, 3) + 3 * a * std::pow(h, 2) * x - 3 * a * h * std::pow(x, 2) + a * std::pow(x, 3) +
               b * std::pow(h, 2) - 2 * b * h * x + b * std::pow(x, 2) - c * h + c * x + d;
    }
}

template <std::size_t N, typename T>
template <std::size_t H>
T DepthProfiler<N, T>::baseIntegral(T x, const Matrix<H, 1, T>& params, T h) const
{
    if (h == 0)
    {
        return ((1.0 / 4) * params.at(0, 0) * std::pow(x, 4)) + ((1.0 / 3) * params.at(1, 0) * std::pow(x, 3)) +
               ((1.0 / 2) * params.at(2, 0) * std::pow(x, 2)) + params.at(3, 0) * x;
    }
    else
    {
        T a = params.at(0, 0);
        T b = params.at(1, 0);
        T c = params.at(2, 0);
        T d = params.at(3, 0);

        // http://www.wolframalpha.com/input/?i=a*h%5E3%2B3*a*h%5E2*x-3*a*h*x%5E2%2Ba*x%5E3%2Bb*h%5E2-2*b*h*x%2Bb*x%5E2%2Bc*x-c*h%2Bd
        return a * std::pow(h, 3) + 3 / 2 * a * std::pow(h, 2) * std::pow(x, 2) - a * h * std::pow(x, 3) +
               1 / 4 * a * std::pow(x, 4) + b * std::pow(h, 2) * x - b * h * std::pow(x, 2) +
               1 / 3 * b * std::pow(x, 3) - c * h * x + 1 / 2 * c * std::pow(x, 2) + d * x;
    }
}

template <std::size_t N, typename T>
template <std::size_t H>
T DepthProfiler<N, T>::baseDerivative(T x, const Matrix<H, 1, T>& params, T h) const
{
    if (h == 0)
    {
        return x * (3 * params.at(0, 0) * x + 2 * params.at(1, 0)) + params.at(2, 0);
    }
    else
    {
        return (h - x) * (3 * params.at(0, 0) * (h - x) - 2 * params.at(1, 0)) + params.at(2, 0);
    }
}

template <std::size_t N, typename T>
template <std::size_t H, std::size_t aH>
T DepthProfiler<N, T>::calculateDerivative(const Matrix<H, 1, T>& input,
                                           const Matrix<aH, 1, T>& params,
                                           std::size_t param_number) const
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

template <std::size_t N, typename T>
template <std::size_t H, std::size_t W>
Matrix<H - 1, 2, T> DepthProfiler<N, T>::calculateDerivative(Matrix<H, W, T>& matrix) const
{
    Matrix<H - 1, 2, T> result;

    for (std::size_t i = 0; i < H - 1; ++i)
    {
        result[i][0] = matrix[i][0];
        result[i][1] = (matrix[i + 1][1] - matrix[i][1]) / (matrix[i + 1][0] - matrix[i][0]);
    }
    return result;
}

template <std::size_t N, typename T>
void DepthProfiler<N, T>::getTransportForAllData() const
{
    if (depth_data_.size() < 2)
    {
        return;
    }

    for (std::size_t i = 0; i < depth_data_.size() - 1; ++i)
    {
        getTransportOverTime(depth_data_.at(i), depth_data_.at(i + 1));
    }
}

template <std::size_t N, typename T>
template <std::size_t H, std::size_t W>
float DepthProfiler<N, T>::getTransportOverTime(const BottomProfile<H, W, T>& profile_first,
                                                BottomProfile<H, W, T>& profile_second) const
{
    std::vector<std::pair<Dune, Dune>> simular_dunes =
        profile_first.getSimularDune(profile_second, BP_DEFAULT_PRECISION);
#if DEPTHPROFILER_DEBUG > 0
    std::cout << "DepthProfiler: Found " << simular_dunes.size() << " simular dunes." << std::endl;
#endif
    if (simular_dunes.size() >= DP_MINIMUM_WAVE_COUNT)
    {
        float total_transport    = 0;
        uint64_t time_difference = std::abs(profile_second.time_ - profile_first.time_);  // In ms.

        for (auto& i : simular_dunes)
        {
            try
            {
                float transport = calculateMovedArea(i.first, i.second);
                total_transport += transport;
            }
            catch (std::exception& e)
            {
#if DEPTHPROFILER_DEBUG > 0
                std::cout << "DepthProfiler: Transport over time error: " << e.what() << std::endl;
#endif
                continue;
            }
        }
        profile_second.average_transport_ =
            (total_transport) / simular_dunes.size() / static_cast<float>(time_difference) * 1000.0;
#if DEPTHPROFILER_DEBUG > 0
        std::cout << "DepthProfiler: Average transport: " << profile_second.average_transport_ << std::endl;
#endif
        return profile_second.average_transport_;
    }
    else
    {
        profile_second.average_transport_ = 0;
    }
    return 0;
}

template <std::size_t N, typename T>
float DepthProfiler<N, T>::calculateMovedArea(const Dune& dune, const Dune& other_dune) const
{
    // Calculate the how much our dune has moved across the x axis.
    T x_diff            = other_dune.start_x_ - dune.start_x_;
    T max_guess         = dune.end_x_ - other_dune.start_x_;
    T inital_guess      = (max_guess + x_diff) / 2;
    auto solution_space = std::make_pair(0, max_guess + x_diff);

    // Calculate intersection between the dunes.a
    T intersection = newtonsMethod(dune.signature_, other_dune.signature_, solution_space, inital_guess, x_diff);

    // Calculate how much of the dune has stayed in place
    float area_of_a = 0;
    float area_of_b = 0;

    if (x_diff >= 0)
    {
        area_of_a = calculateArea(dune.signature_, intersection, (dune.end_x_ - dune.start_x_));
        area_of_b = calculateArea(other_dune.signature_, 0, intersection + x_diff);
    }
    else
    {
        area_of_a =
            calculateArea(other_dune.signature_, intersection + x_diff, (other_dune.end_x_ - other_dune.start_x_));
        area_of_b = calculateArea(dune.signature_, 0, intersection);
    }

    float moved_area = std::abs(other_dune.surface_area_ - (area_of_a + area_of_b));
#if DEPTHPROFILER_DEBUG > 0
    std::cout << "DepthProfiler: Moved area: " << moved_area << " a: " << area_of_a << " b: " << area_of_b
              << " dune_r: " << other_dune.surface_area_ << " intersec: " << intersection << std::endl;
#endif
    return moved_area;
}

template <std::size_t N, typename T>
T DepthProfiler<N, T>::calculateArea(const Matrix<4, 1, T>& params, T start, T end) const
{
    T y1 = baseIntegral(start, params);
    T y2 = baseIntegral(end, params);
    return std::abs(y2 - y1);
}

template <std::size_t N, typename T>
template <std::size_t H, std::size_t W>
std::vector<std::size_t> DepthProfiler<N, T>::findPeaksAndValleys(const Matrix<H, W, T>& matrix,
                                                                  const Matrix<H - 1, W - 1, T>& derivative,
                                                                  std::size_t minimal_x_diff) const
{
    std::vector<std::size_t> result;
    std::vector<std::size_t> straights;
    std::size_t last_x   = 0;
    float sum_derivative = 0;

    for (std::size_t i = 2; i < H - 3; ++i)
    {
        if (i >= 5)
        {
            straights.erase(std::remove(straights.begin(), straights.end(), i - 5), straights.end());
        }

        // We found a zero measurement do not check this 'i' value.
        if (matrix[i - 2][1] == 0 || matrix[i - 1][1] == 0 || matrix[i][1] == 0 || matrix[i + 1][1] == 0 ||
            matrix[i + 2][1] == 0)
        {
#if DEPTHPROFILER_DEBUG > 2
            std::cout << "DepthProfiler (" << i << "): Zero found " << std::endl;
#endif
            straights.push_back(i);
            continue;
        }

        // Sum the amount the derivative increases.
        sum_derivative = 0;
        for (std::size_t k = 0; k < 5; ++k)
        {
            sum_derivative += derivative[i - 2 + k][1];
        }

        // Does it increase by a small amount, then is properly a straight line.
        if (std::abs(sum_derivative) < 0.005)
        {
            straights.push_back(i);
            // Do we have multiple occuring straight lines?
            if (!(i >= 5 && straights.size() >= 1 && straights.size() < 3))
            {
#if DEPTHPROFILER_DEBUG > 2
                std::cout << "DepthProfiler (" << i << "): Straight line" << std::endl;
#endif
                continue;
            }
#if DEPTHPROFILER_DEBUG > 2
            else
            {
                std::cout << "DepthProfiler (" << i << "): Possible straight line" << std::endl;
            }
#endif
        }

        // Check is it is a peak or valley.
        if (derivative[i - 2][1] >= 0 && derivative[i - 1][1] >= 0 && derivative[i][1] <= 0 &&
            derivative[i + 1][1] <= 0 && derivative[i + 2][1] <= 0 && i - last_x > minimal_x_diff)
        {
#if DEPTHPROFILER_DEBUG > 2
            std::cout << "DepthProfiler: Positive to negative: " << i << std::endl;
#endif
            straights.erase(std::remove(straights.begin(), straights.end(), i), straights.end());
            last_x = i;
            result.push_back(i);
        }
        else if (derivative[i - 2][1] <= 0 && derivative[i - 1][1] <= 0 && derivative[i][1] >= 0 &&
                 derivative[i + 1][1] >= 0 && derivative[i + 2][1] >= 0 && i - last_x > minimal_x_diff)
        {
#if DEPTHPROFILER_DEBUG > 2
            std::cout << "DepthProfiler: Negative to positive: " << i << std::endl;
#endif
            straights.erase(std::remove(straights.begin(), straights.end(), i), straights.end());
            last_x = i;
            result.push_back(i);
        }
#if DEPTHPROFILER_DEBUG > 2
        std::cout << "DepthProfiler (" << i << "): Something else? " << derivative[i - 2][1] << ","
                  << derivative[i - 1][1] << "," << derivative[i][1] << "," << derivative[i + 1][1] << ","
                  << derivative[i + 2][1] << std::endl;
#endif
    }
#if DEPTHPROFILER_DEBUG > 0
    std::cout << "DepthProfiler: Found: " << result.size() << " peaks or valleys." << std::endl;
#endif
    return result;
}

template <std::size_t N, typename T>
template <std::size_t H, std::size_t W>
std::vector<std::pair<std::size_t, std::size_t>>
DepthProfiler<N, T>::classifyWave(const Matrix<H, W, T>& afgeleide,
                                  const std::vector<std::size_t>& peaks_and_valleys) const
{
    std::vector<std::pair<std::size_t, std::size_t>> result;
    if (peaks_and_valleys.size() < 3)
    {
        return result;
    }

    std::size_t i = 0;
    do
    {
        // Find a valley -> peak -> valley order.
        if (isPeak(afgeleide, peaks_and_valleys[i]) && !isPeak(afgeleide, peaks_and_valleys[i + 1]) &&
            isPeak(afgeleide, peaks_and_valleys[i + 2]))
        {
#if DEPTHPROFILER_DEBUG > 1
            std::cout << "DepthProfiler: Wave from: " << peaks_and_valleys[i] << " to: " << peaks_and_valleys[i + 2]
                      << "   from_x: " << afgeleide.at(peaks_and_valleys[i], 0)
                      << " to_x: " << afgeleide.at(peaks_and_valleys[i + 2], 0) << std::endl;
            std::cout << "---" << std::endl;
#endif
            result.push_back(std::make_pair(peaks_and_valleys[i], peaks_and_valleys[i + 2]));
        }
        i++;
    } while (i < peaks_and_valleys.size() - 2);

#if DEPTHPROFILER_DEBUG > 0
    std::cout << "DepthProfiler: Found: " << result.size() << " Waves" << std::endl;
#endif
    return result;
}

template <std::size_t N, typename T>
template <std::size_t H, std::size_t W>
void DepthProfiler<N, T>::shiftWaveToZero(Matrix<H, W, T>& matrix, std::size_t dune_size) const
{
    T start_x = matrix.at(0, 0);
    for (std::size_t i = 0; i < dune_size; ++i)
    {
        matrix.at(i, 0) -= start_x;
    }
}

template <std::size_t N, typename T>
template <std::size_t H, std::size_t W>
bool DepthProfiler<N, T>::isPeak(const Matrix<H, W, T>& derivative, std::size_t index) const
{
    assert(index + 2 <= H);
    assert(index >= 2);
#if DEPTHPROFILER_DEBUG > 1
    std::cout << "DepthProfiler: isPeak(" << index
              << "):" << (derivative[index - 2][1] > 0 && derivative[index - 1][1] > 0 && derivative[index][1] < 0 &&
                          derivative[index + 1][1] < 0 && derivative[index + 2][1] <= 0)
              << std::endl;
#endif
    return derivative[index - 2][1] >= 0 && derivative[index - 1][1] >= 0 && derivative[index][1] <= 0 &&
           derivative[index + 1][1] <= 0 && derivative[index + 2][1] <= 0;
}
}
