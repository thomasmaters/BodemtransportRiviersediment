/*
 * OnderzoekGnuplotMain.cpp
 *
 *  Created on: 1 Aug 2018
 *      Author: Thomas Maters
 *		Email : thomasmaters@hotmail.com (TG.Maters@student.han.nl)
 */

#include "gnuplot-iostream.h"

#include <chrono>
#include <random>

int main(int argc, char **argv)
{
    Gnuplot gp;
    std::vector<std::tuple<std::size_t, std::size_t> > pts_A;
    std::vector<std::vector<std::tuple<std::uint16_t, std::uint16_t, std::uint16_t> > > pts_B;

    //	std::uint16_t size = 1000;
    //	for (std::uint16_t i = 0; i < size; ++i) {
    //		pts_B.resize(size);
    //		for (std::uint16_t j = 0; j < size; ++j) {
    //			pts_B[i].push_back(std::make_tuple(i, j, std::rand() % 5));
    //		}
    //	}
    //
    //	gp << "set xrange[0:1000]\n";
    //	gp << "set yrange[0,1000]\n";
    //	gp << "set zrange[0,5]\n";
    //	gp << "set view 60, 30\n";
    //	gp << "show view\n";
    //	gp << "splot '-' with pm3d\n";
    //	gp.send2d(pts_B);

    gp << "set xrange [0:500]\nset yrange [0:4]\n";

    for (std::size_t k = 0; k < 10; ++k)
    {
        auto start = std::chrono::steady_clock::now();
        pts_A.clear();
        for (std::size_t i = 0; i < 500; ++i)
        {
            pts_A.push_back(std::make_pair(i, std::rand() % 5));
        }
        gp << "clear\n";
        gp << "plot '-' with boxes title 'pts_A'\n";
        gp.send1d(pts_A);
        gp << "e\n";
        gp.flush();
        auto end  = std::chrono::steady_clock::now();
        auto diff = end - start;
        std::cout << std::chrono::duration<double, std::milli>(diff).count() << " ms" << std::endl;
    }

    std::cout << "Press enter to exit." << std::endl;
    std::cin.get();
}
