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
ReturnType hendrik(U1* array)
{
	return static_cast<ReturnType>(sub_hendrik<sizeof(ReturnType)>(array, N));
}

template<typename... types>
class InterfaceBase : public std::tuple<types...>
{
public:
	InterfaceBase()
	{
		data = new U1[total_size];
		data[0] = 0b11010010;
		data[1] = 0b00000100;
		data[2] = 0;
		data[3] = 0;
	};

	virtual~ InterfaceBase()
	{
		delete data;
	}
	constexpr static std::size_t size = sizeof...(types);
	constexpr static std::size_t total_size = size_in_bytes_template_params<types...>();
	U1* data;

	template<typename... args>
	friend std::ostream& operator<<(std::ostream& os, const InterfaceBase<args...>& a)
	{
		os << size_in_bytes_template_params<args...>();
		return os;
	}

	template<std::size_t N>
	typename get_Nth_type<N,types...>::type gettt()
	{
		using returnType = typename get_Nth_type<N,types...>::type;
	    std::cout << "Pos: " << posOfArgument<N,types...>() << std::endl;
		return hendrik<posOfArgument<N,types...>(), returnType>(data);
	}

//	template<std::size_t N>
//	U1* get()
//	{
//		return &data[posOfArgument<N,types...>()];
//	}
//
//	void set()
//	{
//
//	}
};

template<typename... Args>
std::istream& operator>>(std::istream& is, InterfaceBase<Args...> a)
{
	U1* buffer = new U1[a.total_size];
//	is.read(buffer, a.total_size);
	is >> buffer;
	std::cout << "buf" << buffer[0] << std::endl;
//	delete a.data;
//	a.data = buffer;
//	std::cout << "new buf" << a.data[0] << std::endl;
	return is;
}

#endif /* INTERFACE_HPP_ */
