/*
 * DepthProfiler.cpp
 *
 *  Created on: 2 Jul 2018
 *      Author: Thomas Maters
 *		Email : thomasmaters@hotmail.com (TG.Maters@student.han.nl)
 */
namespace Controller
{
template <std::size_t N, typename T>
DepthProfiler<N, T>::DepthProfiler() : send_transport_updates_(true)
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
        auto afgeleide                             = calculateDerivative(matrix);
        std::vector<std::size_t> peaks_and_valleys = findPeaksAndValleys(afgeleide, 1);
        std::vector<std::pair<std::size_t, std::size_t>> waves = classifyWave(afgeleide, peaks_and_valleys);

        for (std::size_t i = 0; i < waves.size(); ++i)
        {
            Dune dune;
            dune.size_index_  = waves[i].second - waves[i].first;
            dune.start_index_ = waves[i].first;
            dune.start_x_     = matrix.at(dune.start_index_, 0);
            dune.end_x_       = matrix.at(waves[i].second, 0);

            // TODO: change this magic number.
            if (dune.size_index_ > 60)  // We have a big wave, just skip it. .
            {
                continue;
            }
            if (dune.start_index_ + 60 > N)  // Our wave doesn't fit in matrix, this is really bad coding.
            {
                continue;
            }
            Matrix<60, 2, float> single_dune_matrix(matrix, dune.start_index_, 0);

            // Translate x values so they start from 0. (This helps the Gauss-Newton algorithm find a result.)
            shiftWaveToZero(single_dune_matrix, dune.size_index_);
            try
            {
                Matrix<4, 1, T> params({ 1, 1, 1, 1 });
                gaussNewton(single_dune_matrix, params, dune.size_index_);
                dune.signature_ = params;
                dune.surface_area_ =
                    calculateArea(params, single_dune_matrix.at(0, 0), single_dune_matrix.at(dune.size_index_, 0));
                bottom_profile.dunes_.push_back(dune);
            }
            catch (std::exception& e)
            {
#if DEPTHPROFILER_DEBUG > 0
                std::cout << "DepthProfiler: Add proccessed point error: " << e.what() << " peaks or valleys."
                          << std::endl;
#endif
            }
        }

        // Calculate the transport!
        if (depth_data_.size() > 0)
        {
            getTransportOverTime(depth_data_.back(), bottom_profile);
            if (send_transport_updates_)
            {
                sendBottomProfile(bottom_profile);
            }
        }

        depth_data_.push_back(bottom_profile);
    }
    catch (std::exception& e)
    {
#if DEPTHPROFILER_DEBUG > 0
        std::cout << "DepthProfiler: Add proccessed point error: " << e.what() << " peaks or valleys." << std::endl;
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
            new_guess = solution_space.first + (solution_space.second - solution_space.first) / 1.95;
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
void DepthProfiler<N, T>::getTransportOverTime(const BottomProfile<H, W, T>& profile_first,
                                               BottomProfile<H, W, T>& profile_second) const
{
    // TODO: Magic number
    std::vector<std::pair<Dune, Dune>> simular_dunes = profile_first.getSimularDune(profile_second, 2.0);
#if DEPTHPROFILER_DEBUG > 0
    std::cout << "DepthProfiler: Found " << simular_dunes.size() << " simular dunes." << std::endl;
#endif
    if (simular_dunes.size() > 0)
    {
        float total_transport    = 0;
        float total_wave_length  = 0;
        uint64_t time_difference = std::abs(profile_second.time_ - profile_first.time_);  // In ms.

        for (auto& i : simular_dunes)
        {
            try
            {
                float transport = calculateMovedArea(i.first, i.second);
                total_transport += transport;
                total_wave_length += i.first.end_x_ - i.first.start_x_;
            }
            catch (std::exception& e)
            {
#if DEPTHPROFILER_DEBUG > 0
                std::cout << "DepthProfiler: Transport over time error: " << e.what() << std::endl;
#endif
                continue;
            }
        }
        // total_transport / total_wave_length / ms * 1000 = m^2/s
        // TODO: Transport per dune of over de lengte van alle dunes bij elkaar?
        profile_second.average_transport_ = (total_transport) / (total_wave_length) / (float)time_difference * 1000.0;
    }
    else
    {
        profile_second.average_transport_ = 0;
    }
#if DEPTHPROFILER_DEBUG > 0
    std::cout << "DepthProfiler: Average transport: " << profile_second.average_transport_ << std::endl;
#endif
}

template <std::size_t N, typename T>
float DepthProfiler<N, T>::calculateMovedArea(const Dune& dune_left, const Dune& dune_right) const
{
    // Calculate the how much our dune has moved across the x axis.
    T x_diff            = dune_right.start_x_ - dune_left.start_x_;
    T max_guess         = (dune_left.end_x_ - dune_right.start_x_);
    T inital_guess      = (max_guess + x_diff) / 2;
    auto solution_space = std::make_pair(0, max_guess + x_diff);

    // Calculate intersection between the dunes.
    T root = newtonsMethod(dune_left.signature_, dune_right.signature_, solution_space, inital_guess, x_diff);

    // Calculate how much of the dune has stayed in place
    float area_of_a  = calculateArea(dune_left.signature_, root, (dune_left.end_x_ - dune_left.start_x_));
    float area_of_b  = calculateArea(dune_right.signature_, 0, root + x_diff);
    float moved_area = dune_right.surface_area_ - (area_of_a + area_of_b);

    if (moved_area < 0)
    {
        throw std::runtime_error("Moved dune area is smaller then 0.");
    }

    //    std::cout << "DuneLeft area: " << area_of_a << " DuneRight area: " << area_of_b << " DuneRight surface: " <<
    //    dune_right.surface_area_ << std::endl;
    //    std::cout << "Duneleft: " << dune_left.signature_ << " Dune right: " << dune_right.signature_ << std::endl;
    //    std::cout << "Moved area: " << moved_area << std::endl;

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
std::vector<std::size_t> DepthProfiler<N, T>::findPeaksAndValleys(const Matrix<H, W, T>& derivative,
                                                                  std::size_t minimal_x_diff) const
{
    std::vector<std::size_t> result;
    float last_found_x = 0;
    for (std::size_t i = 0; i < H - 1; ++i)
    {
        if (((derivative[i][1] < 0 && derivative[i + 1][1] > 0) ||
             (derivative[i][1] > 0 && derivative[i + 1][1] < 0)) &&
            abs(derivative[i][0] - last_found_x) > minimal_x_diff)
        {
            last_found_x = derivative[i][0];
            result.push_back(i);
        }
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
    for (std::size_t i = 0; i < peaks_and_valleys.size() - 3; ++i)
    {
        // Find a valley -> peak -> valley order.
        if (!isPeak(afgeleide, peaks_and_valleys[i]) && isPeak(afgeleide, peaks_and_valleys[i + 1]) &&
            !isPeak(afgeleide, peaks_and_valleys[i + 2]))
        {
            result.push_back(std::make_pair(peaks_and_valleys[i], peaks_and_valleys[i + 2]));
#if DEPTHPROFILER_DEBUG > 1
            std::cout << "DepthProfiler: Wave from: " << peaks_and_valleys[i] << " to: " << peaks_and_valleys[i + 2]
                      << "   from_x: " << afgeleide.at(peaks_and_valleys[i], 0)
                      << " to_x: " << afgeleide.at(peaks_and_valleys[i + 2], 0) << std::endl;
#endif
        }
    }

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
    for (std::size_t i = 0; i < dune_size + 1; ++i)
    {
        matrix.at(i, 0) -= start_x;
    }
}

template <std::size_t N, typename T>
template <std::size_t H, std::size_t W>
bool DepthProfiler<N, T>::isPeak(const Matrix<H, W, T>& derivative, std::size_t index) const
{
    assert(index + 2 < H);
    return derivative.at(index, 1) > derivative.at(index + 1, 1) &&
           derivative.at(index + 1, 1) > derivative.at(index + 2, 1);
}
}
