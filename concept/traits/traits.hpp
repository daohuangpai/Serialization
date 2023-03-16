#pragma once
#include <string_view>
#include "concept/base/define.h"
namespace traits
{
	/*
	* constexpr static auto a = is_template<std::vector<int>>::value;
	  constexpr static auto b = is_template<int>::value;
	  判断一个类型是不是模板
	*/

	template<typename T>
	struct is_template :public std::false_type {};

	template<template<class> class T, typename ...Arg>
	struct is_template<T<Arg...>> :public std::true_type {};


	

	//判断两个模板是否一致
	template<template<class> class A, template<class> class B>
	struct is_same_template :public std::false_type
	{
	};
	template<template<class> class A>
	struct is_same_template<A, A> :public std::true_type
	{
	};

	template<template<class> class A, template<class> class B>
	INLINE constexpr auto is_same_template_v = is_same_template<A, B>::value;
	//

	template<template<class> class A, class B>
	struct is_template_of :public std::false_type
	{
	};

	template<template<class> class A, typename ...Arg>
	struct is_template_of<A,A<Arg...>> :public std::true_type
	{
	};

	template<template<class> class A, class B>
	INLINE constexpr auto is_template_of_v = is_template_of<A, B>::value;


	/*
	* static constexpr auto v = is_contain_of<int, float,int>;  true
	* static constexpr auto v = is_contain_of<int, float>; false
	* 第一个类型是否被后面的类型包含
	*/
	template<typename element, typename firstElement, typename... otherElements>
	inline constexpr bool is_contain_of = std::is_same_v<element, firstElement> || is_contain_of<element, otherElements...>;

	template<typename element, typename firstElement>
	inline constexpr bool is_contain_of<element, firstElement> = std::is_same_v<element, firstElement>;

	/*
	* 取得第index元素早Args中的位置
	* 	std::cout << typeid(getIndexType<1, int, float>).name() << std::endl;---》float
	    std::cout << typeid(getIndexType<0, int, float>).name() << std::endl;---》int
	*/

	template<size_t index, typename...Args>
	using getIndexType = decltype(std::get<index>(std::tuple<Args...>{}));
}


#define __defineHasClassMember(memberName) \
template<typename T, class = std::void_t<>>\
struct has##memberName :public std::false_type\
{\
};\
template<typename T>\
struct has##memberName<T, std::void_t<typename T::##memberName>> :public std::true_type\
{\
};\
template<typename T>\
INLINE constexpr auto has##memberName##_v = has##memberName##<T>::value;


#define __defineHasClassMemberWithType(memberName,defaultType) \
template<typename T, class = std::void_t<>>\
struct has##memberName :public std::false_type\
{\
using type = defaultType;\
};\
template<typename T>\
struct has##memberName<T, std::void_t<typename T::##memberName>> :public std::true_type\
{\
using type = typename T::##memberName;\
};\
template<typename T>\
INLINE constexpr auto has##memberName##_v = has##memberName##<T>::value;

#define __hasMember(memberName) has##memberName