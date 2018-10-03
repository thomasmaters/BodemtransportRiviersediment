/*
 * Dune.hpp
 *
 *  Created on: 23 Jul 2018
 *      Author: Thomas Maters
 *		Email : thomasmaters@hotmail.com (TG.Maters@student.han.nl)
 */

#ifndef SRC_DUNE_HPP_
#define SRC_DUNE_HPP_

#include "Matrix.hpp"

#include <chrono>
#include <vector>

#define BP_DEFAULT_PRECISION 5

namespace Profiler
{
struct Dune
{
    std::size_t start_index_;  // Start in Matrix
    std::size_t size_index_;   // Size in Matrix

    float start_x_;
    float end_x_;

    Matrix<4, 1, float> signature_;  // Polynomial coefficients.
    float surface_area_;             // Surface area in m^2.
    float transport_ = 0;

    ~Dune()
    {

    }
};

template <std::size_t H, std::size_t W, typename T>
struct BottomProfile
{
    Matrix<H, W, T> raw_data_;
    std::chrono::milliseconds::rep time_;
    std::vector<Dune> dunes_;
    float average_transport_;  // Transport averaged over all the dunes as compared to the last BottomProfile in m^2/s.

    std::vector<std::pair<Dune, Dune>> getSimularDune(const BottomProfile& profile, T precision = BP_DEFAULT_PRECISION) const
    {
        std::vector<std::pair<Dune, Dune>> result;
        for (const Dune& other_dune : profile.dunes_)
        {
            for (const Dune& dune : dunes_)
            {
                if (other_dune.start_index_ >= dune.start_index_ &&  // The dune is to the right of the left dune.
                    other_dune.start_index_ + other_dune.size_index_ <=
                        dune.start_index_ + dune.size_index_ &&  // The dune is to the left of the right dune.
                    std::abs((int32_t)other_dune.size_index_ - (int32_t)dune.size_index_) <=
                        precision &&  // Size in x direction approx equal
                    std::abs(dune.surface_area_ - other_dune.surface_area_) <= precision  // Surface area approx equal
                    )
                {
                    result.push_back(std::make_pair(other_dune, dune));
                    break;
                }
            }
        }
        return result;
    }
    ~BottomProfile(){}
};
} //Namespace Profiler

#endif /* SRC_DUNE_HPP_ */
