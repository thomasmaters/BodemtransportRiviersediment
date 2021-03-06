/*
 * main.cpp
 *
 *  Created on: 5 Jun 2018
 *      Author: Thomas Maters
 *		Email : thomasmaters@hotmail.com (TG.Maters@student.han.nl)
 */

#include <iostream>
#include "Communication/IOHandler.hpp"
#include "Controller/DeltaT100/DeltaT100Controller.hpp"

int main(int argc, char* argv[])
{
    try
    {
        if (argc >= 4)
        {
            std::string arg1 = std::string(argv[1]);
            std::string arg2 = std::string(argv[2]);
            std::string arg3 = std::string(argv[3]);
            Controller::DeltaT100::DeltaT100Controller* ptr =
                new Controller::DeltaT100::DeltaT100Controller(arg1, arg2, arg3);
            Communication::IOHandler::getInstance().startIOService();
            int a;
            std::cin >> a;
            Communication::IOHandler::getInstance().stopIOService();
            delete ptr;
        }
    }
    catch (std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }
}
