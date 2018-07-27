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

#include <string>
#include <vector>

struct Dune
{
    std::size_t start_index_;  // Start in Matrix
    std::size_t size_index_;   // Size in Matrix

    float start_x_;

    Matrix<4, 1, float> signature_;  // Polynomial coefficients.
    float surface_area_;

    std::string toString()
    {
        return "start: " + std::to_string(start_index_) + " size: " + std::to_string(size_index_) + " area:" +
               std::to_string(surface_area_);
    }
};

template <std::size_t H, std::size_t W, typename T>
struct BottomProfile
{
    Matrix<H, W, T> raw_data_;
    std::vector<Dune> dunes_;
    float average_transport_;  // Transport averaged over all the dunes as compared to the last BottomProfile.

    // TODO: Magic numbers
    std::vector<std::pair<Dune, Dune>> getSimularDune(const BottomProfile& profile, float precision = 5) const
    {
        std::vector<std::pair<Dune, Dune>> result;
        for (const Dune& other_dune : profile.dunes_)
        {
            for (const Dune& dune : dunes_)
            {
                if (other_dune.start_index_ >= dune.start_index_ &&
                    std::abs((int32_t)other_dune.size_index_ - (int32_t)dune.size_index_) <= precision &&
                    std::abs(dune.surface_area_ - other_dune.surface_area_) <= precision
					)
                {
                    result.push_back(std::make_pair(other_dune, dune));
                }
            }
        }
        return result;
    }
};

#endif /* SRC_DUNE_HPP_ */
