/*
 * main.cpp
 *
 *  Created on: 5 Jun 2018
 *      Author: Thomas Maters
 *		Email : thomasmaters@hotmail.com (TG.Maters@student.han.nl)
 */

#include <exception>
#include <iostream>
#include <cstdint>
#include <fstream>

#include "Types.hpp"
#include "Interface.hpp"

//typedef InterfaceBase<U4,U1,U1,U2,U4,U4,U2,U2,U2,U2,F4,U2,U2,F4,U1,U3> EM2040C;
//typedef InterfaceBase<U4,U1,U1,U2,U4,U4,U2,U2,U2,U2,F4,U2,U2,F4,U1/*,U3*/> EM2040C;
typedef InterfaceBase<U2,U2> EM2040C;

int main(int argc, char **argv) {
	try {
		std::cout << "asdf" << std::endl;
		EM2040C asdf;
		std::cout << "Total size: " << asdf << std::endl;
		std::cout << asdf.gettt<0>() << std::endl;
		std::cout << asdf.gettt<1>() << std::endl;
		std::ifstream input("input.bin", std::ios::in | std::ios::binary);
		input >> asdf;
		std::cout << asdf.gettt<0>() << std::endl;
		std::cout << asdf.gettt<1>() << std::endl;
	} catch (std::exception& e) {
		std::cerr << e.what() << std::endl;
	}
}





