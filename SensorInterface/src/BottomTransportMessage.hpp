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
    // Byte 6-15: empty, 00 hex
    // Byte 16-xxx: amountdunes * [a(float),b(float),c(float),d(float),transport(float), x_from(float), x_to(float)]
    // Byte xxx+1: hex BB, dex 187

    BottomTransportMessage() : SensorMessage(command_length_)
    {
        data_[0]               = 0xAA;
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
        return getFloatFromData(2);
    }

    void setAverageTransport(float value)
    {
        setFloatToData(2, value);
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
                dune[j][0] = getFloatFromData(array_ptr);
//                std::cout << BOTTOM_TRANSPORT_HEADER_SIZE + i * 28 + j * 4 << " -> " << dune[j][0] << std::endl;
//                std::cout << BOTTOM_TRANSPORT_HEADER_SIZE + i * 28 + j * 4 << " -> " << (int)data_[array_ptr] << std::endl;
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
                setFloatToData(BOTTOM_TRANSPORT_HEADER_SIZE + i * 28 + j * 4, value.at(i).signature_.at(j, 0));
            }
            setFloatToData(BOTTOM_TRANSPORT_HEADER_SIZE + i * 28 + 16, value.at(i).transport_);
            setFloatToData(BOTTOM_TRANSPORT_HEADER_SIZE + i * 28 + 20, value.at(i).start_x_);
            setFloatToData(BOTTOM_TRANSPORT_HEADER_SIZE + i * 28 + 24, value.at(i).end_x_);
        }
    }

  private:
    float getFloatFromData(std::size_t index)
    {
//        return (data_[index+3] << 24) | (data_[index + 2] << 16) | (data_[index + 1] << 8) | data_[index];
        float result;
        memcpy(&result, &(data_[index]), sizeof(float));
        return result;
    }

    void setFloatToData(std::size_t index, float value)
    {
        memcpy(&(data_[index]), &value, sizeof(value));
    }
};
}

#endif /* SRC_BOTTOMTRANSPORTMESSAGE_HPP_ */
