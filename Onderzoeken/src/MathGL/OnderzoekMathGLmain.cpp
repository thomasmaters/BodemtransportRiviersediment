/*
 * OnderzoekMathGLmain.cpp
 *
 *  Created on: 1 Aug 2018
 *      Author: Thomas Maters
 *		Email : thomasmaters@hotmail.com (TG.Maters@student.han.nl)
 */

#include <mgl2/base.h>
#include <mgl2/data.h>
#include <mgl2/datac.h>
#include <mgl2/mgl.h>
#include <mgl2/define.h>
#include <mgl.h>
#include <mgl2/canvas.h>

#include <random>
#include <chrono>
#include <iostream>

int main(int argc, char **argv) {

	mglData y(2,1);
	mglGraph gr;
	gr.Title("TEST");
	gr.SetRange('x', 0, 500);
	gr.SetRange('y', 0, 4);
	gr.SetOrigin(0,0,0);
	gr.Axis();
	for (std::size_t j = 0; j < 10; ++j) {
		mglData dat_y(500,1);
	    auto start = std::chrono::steady_clock::now();
		for (std::size_t i = 0; i < 500; ++i)
		{
			dat_y.SetVal(std::rand() % 5, i, 0, 0);
		}
		gr.Bars(dat_y);
		gr.WriteFrame("OnderzoekMathGLgr1.png");
	    auto end = std::chrono::steady_clock::now();
	    auto diff = end - start;
	    std::cout << std::chrono::duration <double, std::milli> (diff).count() << " ms" << std::endl;
	}

}
