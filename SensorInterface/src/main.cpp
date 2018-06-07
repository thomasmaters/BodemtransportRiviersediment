/*
 * main.cpp
 *
 *  Created on: 5 Jun 2018
 *      Author: Thomas Maters
 *		Email : thomasmaters@hotmail.com (TG.Maters@student.han.nl)
 */

#include <exception>
#include <bitset>
#include <iostream>
#include <fstream>
#include <tuple>
#include <cstdint>

#include "Types.hpp"

#if __cplusplus == 201103L
	template <bool... b> struct all_el_arithmetic;
	template <bool... tail>
	struct all_el_arithmetic<true, tail...> : all_el_arithmetic<tail...> {};
	template <bool... tail>
	struct all_el_arithmetic<false, tail...> : std::false_type {};
	template <> struct all_el_arithmetic<> : std::true_type {};
	template<typename... types>
	using all_is_arithmetic = all_el_arithmetic<std::is_arithmetic<types>::value...>;
#endif

template<typename... types>
class frituur : public std::tuple<types...>
{
	static_assert(all_is_arithmetic<types...>::value, "Given types are not (all) arithmetic.");
	typedef std::tuple<types...> InheretedType;
public:
	constexpr frituur() : InheretedType(), initialized(false){};

	template<std::size_t idx>
	typename std::tuple_element<idx, std::tuple<types...> >::type const& get()
	{
		return std::get<idx>(*this);
	}

private:
	bool initialized;
};

typedef frituur<U4,U1,U1,U2,U4,U4,U2,U2,U2,U2,F4,U2,U2,F4,U1,U3> EM2040C;
//typedef frituur<U4,U1,U1,U2,U4,U4,U2,U2,U2,U2,F4,U2,U2,F4,U1/*,U3*/> EM2040C;

int main(int argc, char **argv) {
	try {
		std::cout << sizeof(U3_t) << " " << std::is_arithmetic<U3_t>::value << " " << std::is_arithmetic<short>::value << std::endl;
		EM2040C test_frituur;
		std::get<0>(test_frituur);
		test_frituur.get<0>();

		std::ifstream test("input.bin", std::ios::in | std::ios::binary);
		uint16_t takelwagen;
		test >> std::hex >> takelwagen;
		std::cout << takelwagen << std::endl;
		EM2040C kaas;
//		kaas.read(test);
//		std::cout << std::to_string(kaas.a) << std::endl;
//		kaas.write(std::cout);
	} catch (std::exception& e) {
		std::cerr << e.what() << std::endl;
	}
}





