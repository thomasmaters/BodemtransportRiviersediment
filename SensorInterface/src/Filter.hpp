/*
 * MatrixFilter.hpp
 *
 *  Created on: 13 Jul 2018
 *      Author: Thomas Maters
 *		Email : thomasmaters@hotmail.com (TG.Maters@student.han.nl)
 */

#ifndef SRC_FILTER_HPP_
#define SRC_FILTER_HPP_

#include "Profiler/Matrix.hpp"

#include <cstdint>

/**
 * Base filter class.
 */
class Filter
{
  public:
    template <std::size_t H, std::size_t W, typename T>
    void applyFilter(Matrix<H, W, T>& matrix, std::size_t column = 0) const
    {
    }
};

/**
 * Filters Zero data and interpolates the approx values.
 */
class ZeroFilter : public Filter
{
  public:
    template <std::size_t H, std::size_t W, typename T>
    static void applyFilter(Matrix<H, W, T>& matrix, std::size_t column = 0)
    {
    	std::vector<std::size_t> zeros;
    	std::vector<std::size_t> not_zeros;
        for (std::size_t i = 0; i < H; ++i)
        {
        	if(matrix.at(i,column) == 0)
        	{
        		zeros.push_back(i);
        	}
        	else
        	{
        		not_zeros.push_back(i);
        	}
        }

        float i_diff;
        float v_diff;
        float total_distance;

        while(not_zeros.size() > 1 && zeros.size() > 0)
        {
        	i_diff = not_zeros.at(1) - not_zeros.at(0);
        	v_diff = matrix.at(not_zeros.at(1), column) - matrix.at(not_zeros.at(0), column);

			if((zeros.at(0) < not_zeros.at(0) && zeros.at(0) < not_zeros.at(1)) ||
					(zeros.at(0) > not_zeros.at(0) && zeros.at(0) < not_zeros.at(1)) ||
					(zeros.at(0) > not_zeros.at(0) && zeros.at(0) > not_zeros.at(1) && not_zeros.size() == 2))
			{
				total_distance = std::abs(static_cast<float>(not_zeros.at(1)) - static_cast<float>(zeros.at(0)));
				matrix.at(zeros.at(0), column) = matrix.at(not_zeros.at(1), column) - (v_diff / i_diff) * total_distance;
				zeros.erase(zeros.begin());
			}
			else
			{
				not_zeros.erase(not_zeros.begin());
			}
		}
    }
};

/**
 * Filters peaks that are diverging to much.
 */
class PeakFilter : public Filter
{
public:
    template <std::size_t H, std::size_t W, typename T>
    static void applyFilter(Matrix<H, W, T>& matrix, std::size_t column = 0, float percentage = 25)
    {
        for (std::size_t i = 0; i < H - 1; ++i)
        {
            if (matrix.at(i, column) != 0 && matrix.at(i + 1, column) != 0 &&
                std::abs((matrix.at(i + 1, column) - matrix.at(i, column)) / matrix.at(i, column)) * 100 > percentage)
            {
                matrix.at(i + 1, column) = matrix.at(i, column);
            }
        }
    }
};

class SmoothFilter : public Filter
{
public:
    template <std::size_t H, std::size_t W, typename T>
    static void applyFilter(Matrix<H, W, T>& matrix, std::size_t column = 0, std::size_t kernel_size = 5)
    {
    	for (std::size_t i = 0; i < H - kernel_size; ++i) {
        	float sum = 0;
        	for (std::size_t k = 0; k < kernel_size; ++k) {
        		if(matrix[i+k][column] == 0)
        		{
        			sum = kernel_size * matrix[i][column];
        			break;
        		}
        		sum += matrix[i+k][column];
			}
        	matrix[i][column] = sum / kernel_size;
		}
    }
};

#endif /* SRC_FILTER_HPP_ */
