/*
 * main.cpp
 *
 *  Created on: 5 Jun 2018
 *      Author: Thomas Maters
 *		Email : thomasmaters@hotmail.com (TG.Maters@student.han.nl)
 */

#include "BottomTransportMessage.hpp"
#include "DeltaT100Controller.hpp"
#include "IOHandler.hpp"

#include <iostream>

int main(int argc, char* argv[])
{
    try
    {
        if (argc == 5)
        {
            std::string arg1 = std::string(argv[1]);
            std::string arg2 = std::string(argv[2]);
            std::string arg3 = std::string(argv[3]);
            //            IOHandler::getInstance().startIOService();
            //            std::unique_ptr<Controller::DeltaT100::DeltaT100Controller> ptr =
            //            std::unique_ptr<Controller::DeltaT100::DeltaT100Controller>(new
            //            Controller::DeltaT100::DeltaT100Controller(arg1,arg2,arg3));
            //            Controller::DeltaT100::DeltaT100Controller(arg1,arg2,arg3);
            Controller::DeltaT100::DeltaT100Controller* ptr =
                new Controller::DeltaT100::DeltaT100Controller(arg1, arg2, arg3);
            IOHandler::getInstance().startIOService();
            int a;
            std::cin >> a;
            IOHandler::getInstance().stopIOService();
            delete ptr;
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
