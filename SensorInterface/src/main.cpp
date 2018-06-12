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

#include "SwitchDataCommand.hpp"

int main(int argc, char **argv) {
	try {
		Delta100::SwitchDataCommand header;
		header.setExternalTransmitDelay(100);
		header.setRunMode(Delta100::RunMode::AUTOGAINENABLE | Delta100::RunMode::TVGENABLE);
//		EM2040 jan;
//		std::cout << std::to_string(jan.type) << std::endl;
//		std::cout << jan.kaas <<  std::endl;
//		std::cout << jan.piet.value << std::endl;
		std::ifstream input("input.bin", std::ios::in | std::ios::binary);
//		input >> asdf;
//		std::cout << asdf.gettt<0>() << std::endl;
//		std::cout << asdf.gettt<1>() << std::endl;
	} catch (std::exception& e) {
		std::cerr << e.what() << std::endl;
	}
}





























