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

    bool operator==(const Dune& other) const {
    	return signature_ == other.signature_ && surface_area_ == other.surface_area_;
    }

    ~Dune()
    {

    }
};
} //Namespace Profiler

#endif /* SRC_DUNE_HPP_ */
