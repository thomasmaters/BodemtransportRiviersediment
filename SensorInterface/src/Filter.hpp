/*
 * MatrixFilter.hpp
 *
 *  Created on: 13 Jul 2018
 *      Author: Thomas Maters
 *		Email : thomasmaters@hotmail.com (TG.Maters@student.han.nl)
 */

#ifndef SRC_FILTER_HPP_
#define SRC_FILTER_HPP_

#include "Matrix.hpp"

#include <cstdint>

/**
 * Base filter class.
 */
class Filter
{
  public:
    template <std::size_t H, typename T>
    void applyFilter(Matrix<H, 3, T>& matrix) const
    {
    }
};

/**
 * Filters Zero data and interpolates the approx values.
 */
class ZeroFilter : public Filter
{
  public:
    template <std::size_t H, typename T>
    static void applyFilter(Matrix<H, 3, T>& matrix) const
    {
        bool find_first                  = true;
        std::size_t index_first_not_zero = 0;
        for (std::size_t i = 0; i < H; ++i)
        {
            if (matrix.at(i, 0) != 0 && find_first)
            {
                index_first_not_zero = i;
                find_first           = false;
            }
            else if (matrix.at(i, 0) != 0 && !find_first)
            {
                if (i - index_first_not_zero > 1)
                {
                    float diff0 = matrix.at(i, 0) - matrix.at(index_first_not_zero, 0);
                    float diff2 = matrix.at(i, 1) - matrix.at(index_first_not_zero, 1);

                    for (std::size_t k = index_first_not_zero + 1; k < i; ++k)
                    {
                        matrix.at(k, 0) = matrix.at(i, 0) + diff0 / (i - index_first_not_zero) * (i - k + 1);
                        matrix.at(k, 1) = matrix.at(i, 1) + diff2 / (i - index_first_not_zero) * (i - k + 1);
                    }
                }
                index_first_not_zero = i;
            }
            else if (index_first_not_zero < H - 1 && i == H - 1)
            {
                for (std::size_t k = index_first_not_zero + 1; k < H; ++k)
                {
                    matrix.at(k, 0) = matrix.at(index_first_not_zero, 0);
                    matrix.at(k, 1) = matrix.at(index_first_not_zero, 1);
                }
            }
        }
    }
};

/**
 * Filters peaks that are diverging to much.
 */
class PeakFilter : public Filter
{
    template <std::size_t H, typename T>
    static void applyFilter(Matrix<H, 3, T>& matrix, float percentage = 25) const
    {
        for (std::size_t i = 0; i < H - 1; ++i)
        {
            if (matrix.at(i, 1) != 0 && matrix.at(i + 1, 1) != 0 &&
                std::abs((matrix.at(i + 1, 1) - matrix.at(i, 1)) / matrix.at(i, 1)) * 100 > percentage)
            {
                matrix.at(i + 1, 1) = matrix.at(i, 1);
            }
        }
    }
};

#endif /* SRC_FILTER_HPP_ */
