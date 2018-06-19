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
#include <random>

#include "SwitchDataCommand.hpp"
#include "SensorMessage.hpp"
#include "TCPConnection.hpp"
#include "DeltaT100Controller.hpp"
#include "UDPConnection.hpp"

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
			Controller::DeltaT::DeltaT100Controller asdf(io_service, arg1,arg2,arg3);

//			io_service.run();
//			Controller::DeltaT::DeltaT100Controller controller(arg1,arg2,arg3);
		}
		else
		{
			std::cout << "We missen wat argumenten" << std::endl;
		}
//		asdf
//		Communication::TCP::TCPServer2<Communication::TCP::chat_handler> asdf2 = Communication::TCP::TCPServer2<Communication::TCP::chat_handler>();
		std::cout << "before starting server" << std::endl;
//		asdf2.startServer(12345);
		std::cout << "afster starting server" << std::endl;
		std::thread kaas = std::thread([]{std::cout << "start sleep" << std::endl;std::this_thread::sleep_for(std::chrono::seconds(30)); std::cout << "sleep end" << std::endl;});
		kaas.join();
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





























