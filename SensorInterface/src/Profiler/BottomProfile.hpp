/*
 * BottomProfile.hpp
 *
 *  Created on: 15 Oct 2018
 *      Author: Thomas Maters
 *		Email : thomasmaters@hotmail.com (TG.Maters@student.han.nl)
 */

#ifndef SRC_PROFILER_BOTTOMPROFILE_HPP_
#define SRC_PROFILER_BOTTOMPROFILE_HPP_

#define BP_DEFAULT_PRECISION 0.9f

#include "Matrix.hpp"
#include "Dune.hpp"

#include <vector>
#include <chrono>

namespace Profiler
{

template <std::size_t H, std::size_t W, typename T>
class BottomProfile
{
public:
	BottomProfile();

	virtual ~BottomProfile();

	/**
	 * Finds dunes that are almost equal to each other.
	 * @param profile To compare against.
	 * @param precision Tolerance
	 * @return Vector of pairs of similar dunes.
	 */
    std::vector<std::pair<Dune, Dune>> getSimularDune(const BottomProfile& profile, T precision = BP_DEFAULT_PRECISION) const;
private:
    /**
     * Compares the size along the x axis.
     * @param dune
     * @param other_dune
     * @param precision
     * @return True if higher then precision.
     */
    bool checkSize(const Dune& dune, const Dune& other_dune, T precision = BP_DEFAULT_PRECISION) const;
    /**
     * Compares surface area of the dune.
     * @param dune
     * @param other_dune
     * @param precision
     * @return True if higher then precision.
     */
    bool checkArea(const Dune& dune, const Dune& other_dune, T precision = BP_DEFAULT_PRECISION) const;
    /**
     * Compares the position in the whole measurement.
     * @param dune
     * @param other_dune
     * @param precision
     * @return True if higher then precision.
     */
    bool checkPosition(const Dune& dune, const Dune& other_dune, T precision = BP_DEFAULT_PRECISION) const;
    /**
     * Checks if the dune already has a similar dune.
     * @param dune
     * @param result
     * @return True if already found.
     */
    bool checkAlreadyFound(const Dune& dune, const std::vector<std::pair<Dune, Dune>>& result) const;
public:

    Matrix<H, W, T> raw_data_;
    std::chrono::milliseconds::rep time_;
    std::vector<Dune> dunes_;
    float average_transport_;  // Transport averaged over all the dunes as compared to the last BottomProfile in m^2/s.
};


}// Namespace Profiler

#include "BottomProfile.inc"

#endif /* SRC_PROFILER_BOTTOMPROFILE_HPP_ */
