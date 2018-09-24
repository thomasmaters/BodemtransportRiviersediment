/*
 * BottomTransportMessage.hpp
 *
 *  Created on: 6 Aug 2018
 *      Author: Thomas Maters
 *		Email : thomasmaters@hotmail.com (TG.Maters@student.han.nl)
 */

#ifndef SRC_BOTTOMTRANSPORTMESSAGE_HPP_
#define SRC_BOTTOMTRANSPORTMESSAGE_HPP_

#include "Dune.hpp"
#include "SensorMessage.hpp"

#include <cstring>
#include <cassert>

#define BT_HEADER_SIZE 16
#define BT_MESSAGE_SIZE 2048
#define BT_MESSAGE_START 0
#define BT_MESSAGE_START_VALUE 0xAA //170
#define BT_MESSAGE_END BT_MESSAGE_SIZE - 1
#define BT_MESSAGE_END_VALUE 0xBB //187

#define BT_DUNE_AMOUNT_INDEX 1
#define BT_AVG_TRANSPORT_INDEX 2
#define BT_PING_TIME_INDEX 6

#define BT_DUNE_DATA_SIZE 28
#define BT_DUNE_DATA_TRANSPORT_OFFSET 16
#define BT_DUNE_DATA_XFROM_OFFSET 20
#define BT_DUNE_DATA_XTO_OFFSET 20

namespace Messages
{
/**
 * Message for sending data of the calculated bottom transport.
 */
class BottomTransportMessage : public SensorMessage
{
  public:
    static constexpr std::size_t command_length_ = BT_MESSAGE_SIZE;

    // Byte 0: Hex AA, dec 170, indentifier
    // Byte 1: amount dunes
    // Byte 2-5: average transport
    // Byte 6-13: time of ping
    // Byte 14-15: empty, 00 hex
    // Byte 16-xxx: amountdunes * [a(float),b(float),c(float),d(float),transport(float), x_from(float), x_to(float)]
    // Byte xxx+1: hex BB, dec 187
    BottomTransportMessage() : SensorMessage(command_length_)
    {
        data_[BT_MESSAGE_START] = BT_MESSAGE_START_VALUE;
        data_[BT_MESSAGE_END] 	= BT_MESSAGE_END_VALUE;
    }

    explicit BottomTransportMessage(uint8_t* data) : SensorMessage(data, command_length_)
    {
    }

    BottomTransportMessage(const BottomTransportMessage& rhs) : SensorMessage(rhs)
    {
    }

    explicit BottomTransportMessage(const SensorMessage& rhs) : SensorMessage(rhs)
    {
    }

    BottomTransportMessage& operator=(const BottomTransportMessage& rhs)
    {
        if (this != &rhs)
        {
            SensorMessage::operator=(rhs);
        }
        return *this;
    }

    inline uint8_t getAmoundOfDunes() const
    {
        return data_[BT_DUNE_AMOUNT_INDEX];
    }

    inline void setAmoundOfDunes(uint8_t value)
    {
        data_[BT_DUNE_AMOUNT_INDEX] = value;
    }

    inline float getAverageTransport() const
    {
        return getDataAtIndex<float>(BT_AVG_TRANSPORT_INDEX);
    }

    inline void setAverageTransport(float value)
    {
        setDataToIndex(BT_AVG_TRANSPORT_INDEX, value);
    }

    inline int64_t getTimeOfPing() const
    {
        return getDataAtIndex<int64_t>(BT_PING_TIME_INDEX);
    }

    inline void setTimeOfPing(const int64_t& value)
    {
        setDataToIndex(BT_PING_TIME_INDEX, value);
    }

    /**
     * Gets all the stored dunes as a vector of Matrix data.
     * @return All stored dune data.
     */
    template<std::size_t H>
    std::vector<Matrix<H, 1, float>> getDunes() const
    {
        std::vector<Matrix<H, 1, float>> result;

        for (std::size_t i = 0; i < getAmoundOfDunes(); ++i)
        {
            Matrix<H, 1, float> dune;
            for (std::size_t j = 0; j < dune.getHeight(); ++j)
            {
                std::size_t array_ptr = BT_HEADER_SIZE + i * BT_DUNE_DATA_SIZE + j * sizeof(float);
                dune[j][0]            = getDataAtIndex<float>(array_ptr);
            }
            result.push_back(dune);
        }
        return result;
    }

    /**
     * Sets dune data in this message from a BottomProfile.
     * @param value
     */
    template <std::size_t H, std::size_t W, typename T>
    void setDunes(const BottomProfile<H, W, T>& value)
    {
        setDunes(value.dunes_);
    }

    /**
     * Sets dunes in this message from a vector of dunes.
     * @param value
     */
    void setDunes(const std::vector<Dune>& value)
    {
        setAmoundOfDunes(value.size());
        for (std::size_t i = 0; i < value.size(); ++i)
        {
            for (std::size_t j = 0; j < value.at(i).signature_.getHeight(); ++j)
            {
                setDataToIndex<float>(BT_HEADER_SIZE + i * BT_DUNE_DATA_SIZE + j * sizeof(float), value.at(i).signature_.at(j, 0));
            }
            setDataToIndex<float>(BT_HEADER_SIZE + i * BT_DUNE_DATA_SIZE + BT_DUNE_DATA_TRANSPORT_OFFSET, value.at(i).transport_);
            setDataToIndex<float>(BT_HEADER_SIZE + i * BT_DUNE_DATA_SIZE + BT_DUNE_DATA_XFROM_OFFSET, value.at(i).start_x_);
            setDataToIndex<float>(BT_HEADER_SIZE + i * BT_DUNE_DATA_SIZE + BT_DUNE_DATA_XTO_OFFSET, value.at(i).end_x_);
        }
    }

  private:
    /**
     * Convenience function to get data at an offset in the message buffer and return the requested type.
     * @param Index Buffer offset.
     * @return Value of type T.
     */
    template <typename T>
    T getDataAtIndex(std::size_t index) const
    {
    	//Trying to get data outside of the message buffer.
    	assert(index + sizeof(T) <= BT_MESSAGE_END);

        T result;
        std::memcpy(&result, &(data_[index]), sizeof(T));
        return result;
    }

    /**
     * Convenience function to set data at an offset in the message buffer.
     * @param index Buffer offset.
     * @param value Value to set.
     */
    template <typename T>
    void setDataToIndex(std::size_t index, T value)
    {
    	//Trying to set data outside of the message buffer.
    	assert(index + sizeof(T) <= BT_MESSAGE_END);

        memcpy(&(data_[index]), &value, sizeof(T));
    }
};
}

#endif /* SRC_BOTTOMTRANSPORTMESSAGE_HPP_ */
