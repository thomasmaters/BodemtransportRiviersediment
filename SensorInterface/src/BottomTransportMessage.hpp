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

#include <iomanip>

#define BOTTOM_TRANSPORT_HEADER_SIZE 16

namespace Controller
{
class BottomTransportMessage : public SensorMessage
{
  public:
    static constexpr std::size_t command_length_ = 2048;
    // Byte 0: Hex AA, dec 170, indentifier
    // Byte 1: amount dunes
    // Byte 2-5: average transport
    // Byte 6-13: time of ping
    // Byte 14-15: empty, 00 hex
    // Byte 16-xxx: amountdunes * [a(float),b(float),c(float),d(float),transport(float), x_from(float), x_to(float)]
    // Byte xxx+1: hex BB, dex 187

    BottomTransportMessage() : SensorMessage(command_length_)
    {
        data_[0]                   = 0xAA;
        data_[command_length_ - 1] = 0xBB;
    }

    explicit BottomTransportMessage(uint8_t* data) : SensorMessage(data, command_length_)
    {
    }

    BottomTransportMessage(const BottomTransportMessage& rhs) : SensorMessage(rhs)
    {
    }

    BottomTransportMessage(const SensorMessage& rhs) : SensorMessage(rhs)
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

    uint8_t getAmoundOfDunes()
    {
        return data_[1];
    }

    void setAmoundOfDunes(uint8_t value)
    {
        data_[1] = value;
    }

    float getAverageTransport()
    {
        return getDataAtIndex<float>(2);
    }

    void setAverageTransport(float value)
    {
        setDataToIndex(2, value);
    }

    int64_t getTimeOfPing()
    {
        return getDataAtIndex<int64_t>(6);
    }

    void setTimeOfPing(int64_t value)
    {
        setDataToIndex(6, value);
    }

    std::vector<Matrix<7, 1, float>> getDunes()
    {
        std::vector<Matrix<7, 1, float>> result;
        std::cout << "amount of dunes: " << std::to_string(getAmoundOfDunes()) << std::endl;

        for (std::size_t i = 0; i < getAmoundOfDunes(); ++i)
        {
            Matrix<7, 1, float> dune;
            for (std::size_t j = 0; j < 7; ++j)
            {
                std::size_t array_ptr = BOTTOM_TRANSPORT_HEADER_SIZE + i * 28 + j * 4;
                dune[j][0]            = getDataAtIndex<float>(array_ptr);
            }
            result.push_back(dune);
        }
        return result;
    }

    template <std::size_t H, std::size_t W, typename T>
    void setDunes(const BottomProfile<H, W, T> value)
    {
        setDunes(value.dunes_);
    }

    void setDunes(const std::vector<Dune>& value)
    {
        setAmoundOfDunes(value.size());
        for (std::size_t i = 0; i < value.size(); ++i)
        {
            for (std::size_t j = 0; j < value.at(i).signature_.getHeight(); ++j)
            {
                setDataToIndex<float>(BOTTOM_TRANSPORT_HEADER_SIZE + i * 28 + j * 4, value.at(i).signature_.at(j, 0));
            }
            setDataToIndex<float>(BOTTOM_TRANSPORT_HEADER_SIZE + i * 28 + 16, value.at(i).transport_);
            setDataToIndex<float>(BOTTOM_TRANSPORT_HEADER_SIZE + i * 28 + 20, value.at(i).start_x_);
            setDataToIndex<float>(BOTTOM_TRANSPORT_HEADER_SIZE + i * 28 + 24, value.at(i).end_x_);
        }
    }

  private:
    template <typename T>
    T getDataAtIndex(std::size_t index)
    {
        T result;
        memcpy(&result, &(data_[index]), sizeof(T));
        return result;
    }

    template <typename T>
    void setDataToIndex(std::size_t index, T value)
    {
        memcpy(&(data_[index]), &value, sizeof(T));
    }
};
}

#endif /* SRC_BOTTOMTRANSPORTMESSAGE_HPP_ */
