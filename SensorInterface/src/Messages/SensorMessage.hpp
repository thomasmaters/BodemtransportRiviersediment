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
#include <chrono>
#include <cstdint>

namespace Messages
{
/**
 * Base class that can be converted to every protocol derived from it.
 */
class SensorMessage
{
  public:
    static std::chrono::milliseconds::rep getCurrentTime()
    {
        return std::chrono::duration_cast<std::chrono::milliseconds>(
                   std::chrono::system_clock::now().time_since_epoch())
            .count();
    }

    /**
     * Constructor
     */
    explicit SensorMessage(std::size_t size, std::chrono::milliseconds::rep time = getCurrentTime())
      : size_(size), time_(time)
    {
        if (size_ == 0)
        {
            data_ = new uint8_t[1]{ 0 };
        }
        else
        {
            data_ = new uint8_t[size]{ 0 };
        }
    }

    /**
     * Constructor
     */
    SensorMessage(uint8_t* data, std::size_t size, std::chrono::milliseconds::rep time = getCurrentTime())
      : size_(size), data_(new uint8_t[size]{ 0 }), time_(time)
    {
        std::copy(data, data + size_, data_);
    }

    /**
     * Copy constructor
     */
    SensorMessage(const SensorMessage& rhs) : size_(rhs.size_), data_(new uint8_t[rhs.size_]{ 0 }), time_(rhs.time_)
    {
        std::copy(rhs.data_, rhs.data_ + size_, data_);
    }

    /**
     * Increase the size of a SensorMessage internal buffer, without dataloss.
     * @param size
     */
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
            time_ = rhs.time_;
            size_ = rhs.size_;
            std::copy(rhs.data_, rhs.data_ + size_, data_);
        }
        return *this;
    }

    /**
     * Get pointer to message buffer.
     * @return
     */
    uint8_t* getData() const
    {
        return data_;
    }

    /**
     * Get the size of the message buffer.
     * @return
     */
    std::size_t getDataLength() const
    {
        return size_;
    }

    /**
     * Gets the time this sensormessage was received.
     * @return
     */
    std::chrono::milliseconds::rep getTime() const
    {
        return time_;
    }

    virtual ~SensorMessage()
    {
        delete[] data_;
    }

  protected:
    std::size_t size_;
    uint8_t* data_;
    std::chrono::milliseconds::rep time_;
};
}  // Namespace Messages

#endif /* SRC_SENSORMESSAGE_HPP_ */
