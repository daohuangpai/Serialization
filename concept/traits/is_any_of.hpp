#pragma once
#include <iostream>

namespace traits
{
	template<typename... ThisType>
	struct is_any_of;

	template<typename ThisType>
	struct is_any_of<ThisType> :public std::false_type {};

	//判断类型ThisType是不是后面类型中的一个
	template<typename ThisType, typename CompareType,typename... OtherCompareTypes>
	struct is_any_of<ThisType, CompareType, OtherCompareTypes...> 
		:public std::conditional_t<std::is_same_v<ThisType, CompareType>, std::true_type,is_any_of<ThisType, OtherCompareTypes...>> {};

	template<typename ThisType, typename... CompareTypes>
	constexpr static auto is_any_of_v = is_any_of<ThisType, CompareTypes...>::value;
	//test
	//constexpr static auto value = is_any_of<int, float, int, char>::value;//true
	//constexpr static auto value1 = is_any_of<int, float, double, char>::value;//false

}