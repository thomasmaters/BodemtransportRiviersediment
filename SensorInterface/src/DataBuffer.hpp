/*
 * DataBuffer.hpp
 *
 *  Created on: 26 Jun 2018
 *      Author: Thomas Maters
 *		Email : thomasmaters@hotmail.com (TG.Maters@student.han.nl)
 */

#ifndef SRC_DATABUFFER_HPP_
#define SRC_DATABUFFER_HPP_
#include <cstdint>
#include <iostream>
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
    //    {
    //    	std::cout << __PRETTY_FUNCTION__ << std::endl;
    //    	rhs.data_store_ = std::move(data_store_);
    //    	rhs.reserved_data_store_ = std::move(reserved_data_store_);
    //    }

    DataBuffer& operator=(const DataBuffer& rhs)
    {
        if (this != &rhs)
        {
            rhs.data_store_          = std::move(data_store_);
            rhs.reserved_data_store_ = std::move(reserved_data_store_);
        }
        return *this;
    }
    std::unique_ptr<uint8_t[]>& getFreeStorage(std::size_t size)
    {
        //    	data_store_mutex_.lock();
        if (size < ChunckSize)
        {
            return getFromReservedStore(size);
        }
        else
        {
            // Allocate custom size
            data_store_.emplace_back(
                std::move(store_type(std::make_pair(std::unique_ptr<uint8_t[]>(new uint8_t[size]), size))));
            //        	data_store_mutex_.unlock();
            return data_store_.back().first;
        }
    }

    std::unique_ptr<uint8_t[]>& moveToBuffer(uint8_t* data, std::size_t size)
    {
        //    	data_store_mutex_.lock();
        std::unique_ptr<uint8_t[]>& store = getFreeStorage(size);
        try
        {
            memmove(store.get(), data, size);
            //            delete data;
        }
        catch (std::exception& e)
        {
            std::cerr << "Error: " << e.what() << std::endl;
        }

        //    	data_store_mutex_.unlock();
        return store;
    }

    const store_type& get(std::size_t index) const
    {
        return data_store_.at(index);
    }

    const std::unique_ptr<uint8_t*>& getData(std::size_t index) const
    {
        return data_store_.at(index).first;
    }

    void erase(std::size_t index)
    {
        //    	data_store_mutex_.lock();
        data_store_.erase(data_store_.begin() + index);
        //    	data_store_mutex_.unlock();
    }

    SensorMessage asMessage(std::size_t index) const
    {
        return SensorMessage(&(data_store_.at(index).first.get()[0]), data_store_.at(index).second);
    }

    std::size_t size() const
    {
        return data_store_.size();
    }

    void clear()
    {
        //    	data_store_mutex_.lock();
        data_store_.clear();
        //    	data_store_mutex_.unlock();
    }

    virtual ~DataBuffer()
    {
        data_store_.clear();
        reserved_data_store_.clear();
        // TODO: cleanup vector?
    }

  private:
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

    //    std::mutex data_store_mutex_;
};

#endif /* SRC_DATABUFFER_HPP_ */
