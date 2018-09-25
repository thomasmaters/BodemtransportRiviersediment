/*
 * DataBuffer.hpp
 *
 *  Created on: 26 Jun 2018
 *      Author: Thomas Maters
 *		Email : thomasmaters@hotmail.com (TG.Maters@student.han.nl)
 */

#ifndef SRC_DATABUFFER_HPP_
#define SRC_DATABUFFER_HPP_

#include "Messages/SensorMessage.hpp"

#include <cstdint>
#include <memory>
#include <mutex>
#include <vector>

#define RESERVED_STORE_RESIZE_THRESHOLD 1
#define RESERVED_STORE_RESIZE_TO_AMOUNT 10

typedef std::pair<std::unique_ptr<uint8_t[]>, std::size_t> store_type;
template <std::size_t ChunckSize = 2048>
class DataBuffer
{
  public:
    DataBuffer() : data_store_(std::vector<store_type>()), reserved_data_store_(std::vector<store_type>())
    {
    }

    DataBuffer(const DataBuffer& rhs) = delete;

    DataBuffer& operator=(const DataBuffer& rhs)
    {
        if (this != &rhs)
        {
            rhs.data_store_          = std::move(data_store_);
            rhs.reserved_data_store_ = std::move(reserved_data_store_);
        }
        return *this;
    }

    /**
     * Gets a free space of memory.
     * @param size
     * @return Pointer to the free memory.
     */
    std::unique_ptr<uint8_t[]>& getFreeStorage(std::size_t size)
    {
        if (size < ChunckSize)
        {
            return getFromReservedStore(size);
        }
        else
        {
            // Allocate custom size
            data_store_.emplace_back(
                std::move(store_type(std::make_pair(std::unique_ptr<uint8_t[]>(new uint8_t[size]), size))));
            return data_store_.back().first;
        }
    }

    /**
     * Moves some data of a length to a free space in memory.
     * @param data
     * @param size
     * @return Pointer to the data.
     */
    std::unique_ptr<uint8_t[]>& moveToBuffer(uint8_t* data, std::size_t size)
    {
        std::unique_ptr<uint8_t[]>& store = getFreeStorage(size);
        try
        {
            memmove(store.get(), data, size);
        }
        catch (std::exception& e)
        {

        }

        return store;
    }

    /**
     * Gets data at a index in the storage.
     * @param index
     * @return
     */
    const store_type& get(std::size_t index) const
    {
        return data_store_.at(index);
    }

    /**
     * Gets the data at a index in the storage.
     * @param index
     * @return
     */
    const std::unique_ptr<uint8_t*>& getData(std::size_t index) const
    {
        return data_store_.at(index).first;
    }

    /**
     * Erases data at a index.
     * @param index
     */
    void erase(std::size_t index)
    {
        data_store_.erase(data_store_.begin() + index);
    }

    /**
     * Converts the data stored at an index into a SensorMessage.
     * @param index
     * @return
     */
    Messages::SensorMessage asMessage(std::size_t index) const
    {
        return Messages::SensorMessage(&(data_store_.at(index).first.get()[0]), data_store_.at(index).second);
    }

    /**
     * Returns the size of the data entries stored.
     * @return
     */
    std::size_t size() const
    {
        return data_store_.size();
    }

    /**
     * Clears all the stored data.
     */
    void clear()
    {
        data_store_.clear();
    }

    virtual ~DataBuffer()
    {
        data_store_.clear();
        reserved_data_store_.clear();
    }

  private:
    /**
     * Gets some preallocated storage.
     * @param actual_size
     * @return Pointer to storage.
     */
    std::unique_ptr<uint8_t[]>& getFromReservedStore(std::size_t actual_size = ChunckSize)
    {
        if (reserved_data_store_.size() <= RESERVED_STORE_RESIZE_THRESHOLD)
        {
            expendReservedStore();
        }

        data_store_.emplace_back(std::move(reserved_data_store_.back()));
        reserved_data_store_.resize(reserved_data_store_.size() - 1);
        data_store_.back().second = actual_size;

        return data_store_.back().first;
    }

    /**
     * Preallocate data.
     */
    void expendReservedStore()
    {
        while (reserved_data_store_.size() < RESERVED_STORE_RESIZE_TO_AMOUNT)
        {
            reserved_data_store_.emplace_back(
                store_type(std::move(std::make_pair(std::unique_ptr<uint8_t[]>(new uint8_t[ChunckSize]), ChunckSize))));
        }
    }

  private:
    std::vector<store_type> data_store_;
    std::vector<store_type> reserved_data_store_;
};

#endif /* SRC_DATABUFFER_HPP_ */
