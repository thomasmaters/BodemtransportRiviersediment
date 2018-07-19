/*
 * main.cpp
 *
 *  Created on: 5 Jun 2018
 *      Author: Thomas Maters
 *		Email : thomasmaters@hotmail.com (TG.Maters@student.han.nl)
 */

#include "DeltaT100Controller.hpp"
#include "SensorPing.hpp"

#include <boost/asio.hpp>
#include <boost/format.hpp>
#include <iomanip>
#include <iostream>

int main(int argc, char* argv[])
{
    std::stringstream ss3;
    try
    {
        if (argc == 5)
        {
            boost::asio::io_service io_service;
            std::string arg1 = std::string(argv[1]);
            std::string arg2 = std::string(argv[2]);
            std::string arg3 = std::string(argv[3]);
            std::size_t arg4 = atoi(argv[4]);
            Controller::DeltaT100::DeltaT100Controller asdf(io_service, arg1, arg2, arg3, arg4);
        }
        else
        {
            std::cout << "We missen wat argumenten" << std::endl;
        }
    }
    catch (std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }
}
