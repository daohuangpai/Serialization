#pragma once
#include <concept/traits/is_any_of.hpp>
namespace traits
{
	template<typename type>
	struct is_binary_pointer
		:std::conditional_t<is_any_of_v<std::decay_t<type>,
		     const char*, char*, unsigned char*, const unsigned char*>,
		std::true_type, std::false_type> {};

	template<typename type>
	constexpr static auto is_binary_pointer_v = is_binary_pointer<type>::value;


	template<typename type>
	struct is_length_type
		:std::conditional_t<is_any_of_v<std::decay_t<type>,
		short, unsigned short, int, unsigned int, long, unsigned long, long long, unsigned long long>,
		std::true_type, std::false_type> {};

	template<typename type>
	constexpr static auto is_length_type_v = is_binary_pointer<type>::value;

}