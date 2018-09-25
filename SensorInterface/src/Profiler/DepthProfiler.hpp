/*
 * DepthProfiler.hpp
 *
 *  Created on: 2 Jul 2018
 *      Author: Thomas Maters
 *		Email : thomasmaters@hotmail.com (TG.Maters@student.han.nl)
 */

#ifndef SRC_DEPTHPROFILER_HPP_
#define SRC_DEPTHPROFILER_HPP_

#define MAX_ITERATIONS 100
#define STEP_SIZE 1e-5
#define PRECISION 1e-5
#define EPSILON 1e-14
#define DEPTHPROFILER_DEBUG 1

#include "../DataBuffer.hpp"
#include "DepthProfilerProxy.hpp"
#include "Dune.hpp"
#include "Matrix.hpp"

#include <chrono>
#include <vector>
#if DEPTHPROFILER_DEBUG > 0
#include <iostream>
#endif

namespace Controller
{
template <std::size_t N, typename T = float>
class DepthProfiler : public DepthProfilerProxy
{
    static_assert(std::is_arithmetic<T>::value, "Type must be aritmatic.");

  public:
    /**
     * Constructor
     */
    DepthProfiler();

    /**
     * Destructor
     */
    virtual ~DepthProfiler();

    /**
     * Add a unfilterd matrix of points to calculate the transport of.
     * It will apply a couple of filters before calling addProcessedPoint().
     * @param matrix A unfilterd matrix.
     * @param time of ping
     */
    void addRawPoint(Matrix<N, 3, T>& matrix, std::chrono::milliseconds::rep time = 0);

    /**
     * Indentifies all the dunes in the bottom profile by:
     * 1. Calculating its derivative.
     * 2. Finding its peaks and valleys.
     * 3. Identifying 'waves' (valley -> peak -> valley).
     * 4. Use gaussNewton algorithme on each 'Wave'.
     * 5. Calculate its surface area.
     * @param matrix A filtered matrix.
     * @param time Of ping.
     */
    void addProcessedPoint(Matrix<N, 3, T>& matrix, std::chrono::milliseconds::rep time = 0);

    /**
     * Enables if a udp packet will be send after calculating the transport of a bottom profile.
     * @param state
     */
    inline void setTransportUpdateEnabled(bool state)
    {
        send_transport_updates_ = state;
    }

  private:
    /**
     * Tries to find a where the polynomial intersects 0 using NewtonsMethod.
     * https://en.wikipedia.org/wiki/Newton%27s_method
     * @param params Polynomial coefficients.
     * @param inital_guess Start guess
     * @param x_shift How much the x-axis will be shifted to calculate a(x-h)^3 + b(x-h)^2 + c(x-h) + d
     * @param precision How precise the solution has to be.
     * @return x where polynomial intersects 0.
     */
    template <std::size_t H>
    T newtonsMethod(const Matrix<H, 1, T>& params,
                    T inital_guess  = 1,
                    T x_shift       = 0,
                    float precision = PRECISION) const;

    /**
     * Tries to find a where the polynomial intersects 0 using NewtonsMethod.
     * https://en.wikipedia.org/wiki/Newton%27s_method
     * But it uses 2 polynomials where the second polynomial is shifted along the x-axis.
     * @param params Polynomial coefficients.
     * @param params2 Polynomial coefficients.
     * @param solution_space Range the solution can be in.
     * @param inital_guess Start guess.
     * @param x_shift How much the x-axis will be shifted for the second polynomial to calculate a(x-h)^3 + b(x-h)^2 +
     * c(x-h) + d
     * @param precision How precise the solution has to be.
     * @return x where polynomial intersects 0.
     */
    template <std::size_t H>
    T newtonsMethod(const Matrix<H, 1, T>& params,
                    const Matrix<H, 1, T>& params2,
                    const std::pair<float, float>& solution_space,
                    T inital_guess  = 1,
                    T x_shift       = 0,
                    float precision = PRECISION) const;

    /**
     * Solve the non-linear least square problem using the Gauss-Newton algorithme.
     * https://en.wikipedia.org/wiki/Gauss%E2%80%93Newton_algorithm
     * @param inputs Matrix of points.
     * @param params Initial polynomial coefficients.
     * @param iterations How many points to use for calculating the polynomial coefficients.
     * @param precision How precies to calculate the coefficients.
     */
    template <std::size_t H, std::size_t W, std::size_t aH>
    void gaussNewton(const Matrix<H, W, T>& inputs,
                     Matrix<aH, 1, T>& params,
                     std::size_t iterations = H,
                     float precision        = 1e-8) const;

    /**
     * Function that the GaussNewton tries to calculate the parameters for by calculating y = ax^3 + bx^2 + cx + d.
     * @param input Matrix of input values. input.at(0,0) has to be the current x value.
     * @param params Polynomial coefficients.
     * @return y
     */
    template <std::size_t H, std::size_t aH>
    T baseFunction(const Matrix<H, 1, T>& input, const Matrix<aH, 1, T>& params) const;

    /**
     * Function that the GaussNewton tries to calculate the parameters for by
     * calculating y = a(x-h)^3 + b(x-h)^2 + c(x-h) + d if h != 0 otherwise y = ax^3 + bx^2 + cx + d.
     * @param x
     * @param params Polynomial coefficients.
     * @param h Shift along x-axis.
     * @return y
     */
    template <std::size_t H>
    T baseFunction(T x, const Matrix<H, 1, T>& params, T h = 0) const;

    /**
     * Calculates the integral of y = ax^3 + bx^2 + cx + d.
     * @param x
     * @param params Polynomial coefficients.
     * @param h Shift along x-axis.
     * @return y
     */
    template <std::size_t H>
    T baseIntegral(T x, const Matrix<H, 1, T>& params, T h = 0) const;

    /**
     * Calculates the derivative of y = ax^3 + bx^2 + cx + d.
     * @param x
     * @param params Polynomial coefficients.
     * @param h Shift along x-axis.
     * @return y
     */
    template <std::size_t H>
    T baseDerivative(T x, const Matrix<H, 1, T>& params, T h = 0) const;

    /**
     * Calculated derivative using: (y1 - y0)/(x1-x0).
     * But when doing so, it will change one parameter with a slight amount (DERIV_STEP).
     * @param input Matrix of input values. input.at(0,0) has to be the current x value.
     * @param params Polynomial coefficients.
     * @param param_number Which coefficient to slightly change.
     * @return derivative.
     */
    template <std::size_t H, std::size_t aH>
    T calculateDerivative(const Matrix<H, 1, T>& input, const Matrix<aH, 1, T>& params, std::size_t param_number) const;

    /**
     * Calculates the derivative of a whole matrix of points by using (y1 - y0)/(x1-x0).
     * @param matrix To calculate the derivative matrix of.
     * @return Derivative matrix.
     */
    template <std::size_t H, std::size_t W>
    Matrix<H - 1, 2, T> calculateDerivative(Matrix<H, W, T>& matrix) const;

    /**
     *	Calculates the transport for all the stored depth data.
     */
    void getTransportForAllData() const;

    /**
     * TODO: maybe return the transport rate as of just setting a field in the second BottomProfile.
     * Calculates the transport between two BottomProfiles.
     * @param profile_first BottomProfile that comes first in time.
     * @param profile_second BottomProfile that comes after the first BottomProfile in time.
     */
    template <std::size_t H, std::size_t W>
    void getTransportOverTime(const BottomProfile<H, W, T>& profile_first,
                              BottomProfile<H, W, T>& profile_second) const;

    /**
     * Calculates how much surface area has been moved between two dunes.
     * 1. Calculates the root of intersection between the polynomials through the dunes.
     * 2. Calculates area from root to end first dune.
     * 3. Calculates area from 0 to root + shift across x-axis.
     * 4. Subtracts surface areas from the total dunes surface area.
     * @param dune_left
     * @param dune_right
     * @return Surface area that moved.
     */
    float calculateMovedArea(const Dune& dune_left, const Dune& dune_right) const;

    /**
     * Calculates the surface area underneath a polynomial using: y = Integral(end) - Integral(start).
     * @param params Polynomial coefficients.
     * @param start Start x.
     * @param end End x.
     * @return Area under polynomial.
     */
    T calculateArea(const Matrix<4, 1, T>& params, T start, T end) const;

    /**
     * Finds peaks and valleys in a matrix of derivatives.
     * @param derivative Matrix of Derivatives.
     * @param minimal_x_diff How much x difference there has to be between detections.
     * @return Vector with indexes in derivative matrix where a peak or valley can be found.
     */
    template <std::size_t H, std::size_t W>
    std::vector<std::size_t> findPeaksAndValleys(const Matrix<H, W, T>& derivative,
                                                 std::size_t minimal_x_diff = 0) const;

    /**
     * Classifies where a wave (valley -> peak -> valley) starts.
     * @param derivative Matrix of derivatives.
     * @param peaks_and_valleys Indexes where a peak or valley can be found.
     * @return Vector with pairs where a wave starts and ends.
     */
    template <std::size_t H, std::size_t W>
    std::vector<std::pair<std::size_t, std::size_t>>
    classifyWave(const Matrix<H, W, T>& derivative, const std::vector<std::size_t>& peaks_and_valleys) const;

    /**
     * Subtracts the first x value from the rest of the matrix to effectively shift the wave to its origin.
     * @param matrix Matrix to shift to its origin.
     * @param dune_size Amount of values to shift to its origin.
     */
    template <std::size_t H, std::size_t W>
    void shiftWaveToZero(Matrix<H, W, T>& matrix, std::size_t dune_size) const;

    /**
     * Checks if a index in a derivative matrix is a peak or a valley.
     * It does this by comparing derivative(i) > derivative(i+1) > derivative(i+2).
     * @param derivative Matrix of derivatives.
     * @param index Index a peak or valley can be found.
     * @return True if it is a peak, false otherwise.
     */
    template <std::size_t H, std::size_t W>
    bool isPeak(const Matrix<H, W, T>& derivative, std::size_t index) const;

  private:
    std::vector<BottomProfile<N, 3, T>> depth_data_;

    bool send_transport_updates_;
};

} /* namespace Controller */

#include "DepthProfiler.inc"

#endif /* SRC_DEPTHPROFILER_HPP_ */
