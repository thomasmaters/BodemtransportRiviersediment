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
	SensorMessage(std::size_t size): data_(new uint8_t[size]), size_(size)
	{
	}

	SensorMessage(uint8_t data, std::size_t size): data_(new uint8_t[size]), size_(size)
	{
	}

	uint8_t* getData() const
	{
		return &(data_.get()[0]);
	}

	std::size_t getDataLength() const
	{
		return size_;
	}
protected:
	std::shared_ptr<uint8_t[]> data_;
	std::size_t size_;
};



#endif /* SRC_SENSORMESSAGE_HPP_ */
