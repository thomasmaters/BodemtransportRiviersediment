/*
 * Interface.hpp
 *
 *  Created on: 7 Jun 2018
 *      Author: Thomas Maters
 *		Email : thomasmaters@hotmail.com (TG.Maters@student.han.nl)
 */

#ifndef INTERFACE_HPP_
#define INTERFACE_HPP_

#include <cstdint>
#include <iostream>
#include <tuple>
#include <typeinfo>
#include <any>

template<std::size_t N, typename T, typename... types>
struct get_Nth_type
{
    using type = typename get_Nth_type<N - 1, types...>::type;
};

template<typename T, typename... types>
struct get_Nth_type<0, T, types...>
{
    using type = T;
};

template<std::size_t N,typename ...types>
constexpr std::size_t sub_pos()
{
	if constexpr(N >= 1)
		return 0 + sizeof(typename get_Nth_type<N,types...>::type) + sub_pos<N - 1, types...>();
	else
		return sizeof(typename get_Nth_type<N,types...>::type);
}

template<std::size_t N, typename ...Args>
constexpr std::size_t posOfArgument()
{
	return sub_pos<N, Args...>();
}

template<typename ...Args>
constexpr std::size_t size_in_bytes_template_params() {
    return (sizeof(Args) + ... );
}

template<std::size_t k>
constexpr std::size_t sub_hendrik(U1* array, std::size_t N)
{
	return (array[N - 1] << (k - 1) * 8) | sub_hendrik<k-1>(array, N - 1);
}

template<>
constexpr std::size_t sub_hendrik<0>(U1* array, std::size_t N)
{
	return 0;
}

template<std::size_t N, typename ReturnType>
ReturnType reinterpet_array(U1* array)
{
	return static_cast<ReturnType>(sub_hendrik<sizeof(ReturnType)>(array, N));
}

template<typename... types>
class InterfaceBase : public std::tuple<types...>
{
public:
	InterfaceBase()
	{
	};

	virtual~ InterfaceBase()
	{
	}
	constexpr static std::size_t size = sizeof...(types);
	constexpr static std::size_t total_size = size_in_bytes_template_params<types...>();

	template<typename... args>
	friend std::ostream& operator<<(std::ostream& os, const InterfaceBase<args...>& a)
	{
		os << size_in_bytes_template_params<args...>();
		return os;
	}

	template<std::size_t N>
	typename std::tuple_element<N, std::tuple<types...> >::type& get()
	{
		return std::get<N>(*this);
	}

	template<std::size_t N>
	void set(typename get_Nth_type<N,types...>::type value)
	{
		std::get<N>(*this) = std::move(value);
	}
};

template<typename... Args>
std::istream& operator>>(std::istream& is, InterfaceBase<Args...> a)
{
	U1* buffer = new U1[a.total_size];
	is >> buffer;
	return is;
}

#include <vector>
#include <variant>

template<std::size_t N>
class InterfaceBase2
{
public:
	InterfaceBase2()
	{
		data[0] = 41;
		data[1] = 41;
		data[2] = 41;
		data[3] = 0;
		data[4] = 0;
		data[5] = 0;
	}


	std::vector<std::variant<U1,U2,U4>> interperData(std::initializer_list<uint8_t> list)
	{
		std::vector<std::variant<U1,U2,U4>> temp;
		std::size_t offset = 0;

		std::cout << __PRETTY_FUNCTION__ << std::endl;
		for(auto it = list.begin(); it != list.end(); ++it) {
			temp.push_back(get(*it, offset));
			offset += *it;
		}
		return temp;
	}

	template<typename a>
	std::variant<U1,U2,U4> get(a size, std::size_t offset)
	{
		std::cout << __PRETTY_FUNCTION__ << ": " << size << std::endl;
		switch (size) {
			case 1:
				return (U1)(data[offset]);
			case 2:
				return (U2)((data[offset] << 8) | (data[offset + 1]));
			case 3:
				return (U4)((data[offset] << 16) | (data[offset + 1] << 8) | data[offset + 2]);
			case 4:
				return (U4)((data[offset] << 24) | (data[offset + 1] << 16) | (data[offset + 2] << 8) | data[offset + 3]);
			default:
				break;
		}
		throw std::runtime_error("Failed to convert byte array to type of size.");
	}

private:
	std::array<char,N> data;
};








































#endif /* INTERFACE_HPP_ */
