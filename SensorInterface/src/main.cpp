/*
 * main.cpp
 *
 *  Created on: 5 Jun 2018
 *      Author: Thomas Maters
 *		Email : thomasmaters@hotmail.com (TG.Maters@student.han.nl)
 */

#include "DeltaT100Controller.hpp"
#include "TCPConnection.hpp"

#include <random>
#include <iostream>
#include <boost/asio.hpp>

int main(int argc, char *argv[]) {
	try {
		if(argc == 5)
		{
		     std::random_device rd;
		     std::mt19937 generator(rd());

			boost::asio::io_service io_service;
			std::string arg1 = std::string(argv[1]);
			std::string arg2 = std::string(argv[2]);
			std::string arg3 = std::string(argv[3]);
			int loop = std::atoi(argv[4]);
//			Communication::UDP::UDPServerClient kaas(io_service, arg1, arg2, arg3);
			Controller::DeltaT100::DeltaT100Controller asdf(io_service, arg1,arg2,arg3);

//			io_service.run();
//			Controller::DeltaT::DeltaT100Controller controller(arg1,arg2,arg3);
		}
		else
		{
			std::cout << "We missen wat argumenten" << std::endl;
		}
	} catch (std::exception& e) {
		std::cerr << e.what() << std::endl;
	}
}





























