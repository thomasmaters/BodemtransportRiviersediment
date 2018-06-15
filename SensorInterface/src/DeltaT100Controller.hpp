/*
 * DeltaT100Controller.hpp
 *
 *  Created on: 13 Jun 2018
 *      Author: Thomas Maters
 *		Email : thomasmaters@hotmail.com (TG.Maters@student.han.nl)
 */

#ifndef SRC_DELTAT100CONTROLLER_HPP_
#define SRC_DELTAT100CONTROLLER_HPP_

#include "ServerInterface.hpp"

namespace Controller
{
/*
 *
 */
class DeltaT100Controller
{
public:
	DeltaT100Controller();

	void handleResponse(uint8_t* data, std::size_t length)
	{

	}

	virtual ~DeltaT100Controller();
};

} /* namespace Controller */

#endif /* SRC_DELTAT100CONTROLLER_HPP_ */
