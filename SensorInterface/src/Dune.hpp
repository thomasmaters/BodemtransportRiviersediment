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
    std::size_t start_;              // Start in Matrix
    std::size_t size_;               // Size in Matrix
    Matrix<4, 1, float> signature_;  // ax^3 + bx^2 + cx + d values.
    float area_;

    std::string toString()
    {
        return "start: " + std::to_string(start_) + " size: " + std::to_string(size_) + " area:" +
               std::to_string(area_);
    }
};

template <std::size_t H, std::size_t W, typename T>
struct BottomProfile
{
    Matrix<H, W, T> raw_data_;
    std::vector<Dune> dunes_;
};

#endif /* SRC_DUNE_HPP_ */
