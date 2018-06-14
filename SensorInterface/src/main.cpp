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
#include "SensorMessage.hpp"
#include "SensorProxy.hpp"
#include "TCPConnection.hpp"

#include <boost/asio.hpp>

int main(int argc, char **argv) {
	try {
		SensorMessage message(5);
		Delta100::SwitchDataCommand header;
		header.setExternalTransmitDelay(100);
		header.setRunMode(Delta100::RunMode::AUTOGAINENABLE | Delta100::RunMode::TVGENABLE);

		boost::asio::io_service io_service;
//		Communication::TCP::TCPServer asdf(io_service) ;
		Communication::TCP::TCPServer2<Communication::TCP::chat_handler> asdf2 = Communication::TCP::TCPServer2<Communication::TCP::chat_handler>();
		asdf2.startServer(1337);
		io_service.run();

//		EM2040 jan;
//		std::cout << std::to_string(jan.type) << std::endl;
//		std::cout << jan.kaas <<  std::endl;
//		std::cout << jan.piet.value << std::endl;
		std::ifstream input("input.bin", std::ios::in | std::ios::binary);
//		input >> asdf;
//		std::cout << asdf.gettt<0>() << std::endl;
//		std::cout << asdf.gettt<1>() << std::endl;
		Controller::SensorProxy::getInstance();
	} catch (std::exception& e) {
		std::cerr << e.what() << std::endl;
	}
}





























