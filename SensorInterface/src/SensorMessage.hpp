/*
 * SensorMessage.hpp
 *
 *  Created on: 12 Jun 2018
 *      Author: Thomas Maters
 *		Email : thomasmaters@hotmail.com (TG.Maters@student.han.nl)
 */

#ifndef SRC_SENSORMESSAGE_HPP_
#define SRC_SENSORMESSAGE_HPP_

#include <algorithm>
#include <cstdint>

/**
 * Base class that can be converted to every protocol derived from it.
 * Its the users responsability to check the size and to clean the data.
 */
class SensorMessage
{
  public:
    /**
     * Constructor
     */
    explicit SensorMessage(std::size_t size) : size_(size), data_(new uint8_t[size]{ 0 })
    {
    }

    /**
     * Constructor
     */
    SensorMessage(uint8_t* data, std::size_t size) : size_(size), data_(data)
    {
    }

    /**
     * Copy constructor
     */
    SensorMessage(const SensorMessage& rhs) : size_(rhs.size_), data_(nullptr)
    {
        std::copy(rhs.data_, rhs.data_ + size_, data_);
    }

    void increaseSize(std::size_t size)
    {
        if (size <= size_)
        {
            return;
        }
        uint8_t* data = new uint8_t[size]{ 0 };  // alloc new data space
        std::copy(data_, data_ + size, data);    // copy old data to new data
        delete[] data_;                          // Delete old data
        size_ = size;                            // Assign size
        data_ = data;                            // Assign pointer to new data
    }

    SensorMessage& operator=(const SensorMessage& rhs)
    {
        if (this != &rhs)
        {
            size_ = rhs.size_;
            std::copy(rhs.data_, rhs.data_ + size_, data_);
        }
        return *this;
    }

    uint8_t* getData() const
    {
        return data_;
    }

    std::size_t getDataLength() const
    {
        return size_;
    }

    virtual ~SensorMessage()
    {
        delete[] data_;
    }

  protected:
    std::size_t size_;
    uint8_t* data_;
};

#endif /* SRC_SENSORMESSAGE_HPP_ */
