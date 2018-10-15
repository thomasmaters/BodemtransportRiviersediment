/*
 * BottomProfile.hpp
 *
 *  Created on: 15 Oct 2018
 *      Author: Thomas Maters
 *		Email : thomasmaters@hotmail.com (TG.Maters@student.han.nl)
 */

#ifndef SRC_PROFILER_BOTTOMPROFILE_HPP_
#define SRC_PROFILER_BOTTOMPROFILE_HPP_

#define BP_DEFAULT_PRECISION 5
#define BP_DIFFERENCE_PRECISION 0.9f

#include "Matrix.hpp"
#include "Dune.hpp"

#include <iostream>
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

    std::vector<std::pair<Dune, Dune>> getSimularDune(const BottomProfile& profile, T precision = BP_DEFAULT_PRECISION) const;
private:
    bool checkSize(const Dune& dune, const Dune& other_dune, T precision = BP_DIFFERENCE_PRECISION) const;
    bool checkArea(const Dune& dune, const Dune& other_dune, T precision = BP_DIFFERENCE_PRECISION) const;
    bool checkPosition(const Dune& dune, const Dune& other_dune, T precision = BP_DIFFERENCE_PRECISION) const;
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
