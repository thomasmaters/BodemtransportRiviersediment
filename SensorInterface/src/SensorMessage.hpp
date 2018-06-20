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
	SensorMessage(std::size_t size): data(new uint8_t[size]), size(size)
	{
	}

	SensorMessage(uint8_t data, std::size_t size): data(new uint8_t[size]), size(size)
	{
	}

	uint8_t* getData() const
	{
		return &(data.get()[0]);
	}

	std::size_t getDataLength() const
	{
		return size;
	}
protected:
	std::shared_ptr<uint8_t[]> data;
	std::size_t size;
};



#endif /* SRC_SENSORMESSAGE_HPP_ */
