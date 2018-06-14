/*
 * SensorMessage.hpp
 *
 *  Created on: 12 Jun 2018
 *      Author: Thomas Maters
 *		Email : thomasmaters@hotmail.com (TG.Maters@student.han.nl)
 */

#ifndef SRC_SENSORMESSAGE_HPP_
#define SRC_SENSORMESSAGE_HPP_

#include <cstdint>
#include <memory>

class SensorMessage
{
public:
	SensorMessage(std::size_t size): data(new uint8_t[size])
	{
//		static_assert(size > 0, "Value of N is zero or negative!");
	}
protected:
	std::shared_ptr<uint8_t[]> data;
};



#endif /* SRC_SENSORMESSAGE_HPP_ */
