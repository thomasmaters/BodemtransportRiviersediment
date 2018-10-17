/*
 * BottomProfile.cpp
 *
 *  Created on: 15 Oct 2018
 *      Author: Thomas Maters
 *		Email : thomasmaters@hotmail.com (TG.Maters@student.han.nl)
 */

namespace Profiler
{
template <std::size_t H, std::size_t W, typename T>
BottomProfile<H, W, T>::BottomProfile() : time_(0), average_transport_(0)
{
}

template <std::size_t H, std::size_t W, typename T>
BottomProfile<H, W, T>::~BottomProfile()
{
}

template <std::size_t H, std::size_t W, typename T>
std::vector<std::pair<Dune, Dune>> BottomProfile<H, W, T>::getSimularDune(const BottomProfile& profile,
                                                                          T precision) const
{
    std::vector<std::pair<Dune, Dune>> result;
    for (const Dune& other_dune : profile.dunes_)
    {
        for (const Dune& dune : dunes_)
        {
            if (checkSize(dune, other_dune, precision) && checkArea(dune, other_dune, precision) &&
                checkPosition(dune, other_dune, precision) && !checkAlreadyFound(dune, result))
            {
                result.push_back(std::make_pair(other_dune, dune));
                break;
            }
        }
    }
    return result;
}

template <std::size_t H, std::size_t W, typename T>
bool BottomProfile<H, W, T>::checkSize(const Dune& dune, const Dune& other_dune, T precision) const
{
    float factor = ((dune.end_x_ - dune.start_x_) / (other_dune.end_x_ - other_dune.start_x_));
    return factor >= precision && factor <= (2 - precision);
}

template <std::size_t H, std::size_t W, typename T>
bool BottomProfile<H, W, T>::checkArea(const Dune& dune, const Dune& other_dune, T precision) const
{
    assert(dune.surface_area_ != 0);
    assert(other_dune.surface_area_ != 0);
    float factor = std::abs(dune.surface_area_ / other_dune.surface_area_);
    return factor >= precision && factor <= (2 - precision);
}

template <std::size_t H, std::size_t W, typename T>
bool BottomProfile<H, W, T>::checkPosition(const Dune& dune, const Dune& other_dune, T precision) const
{
    float factor = static_cast<float>(other_dune.start_index_) / static_cast<float>(dune.start_index_);
    return factor >= precision && factor <= (2 - precision);
}

template <std::size_t H, std::size_t W, typename T>
bool BottomProfile<H, W, T>::checkAlreadyFound(const Dune& dune, const std::vector<std::pair<Dune, Dune>>& vector) const
{
    for (auto it = vector.begin(); it != vector.end(); ++it)
    {
        if (it->second == dune)
        {
            return true;
        }
    }
    return false;
}

}  // Namespace Profiler
