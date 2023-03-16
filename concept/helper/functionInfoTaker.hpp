#pragma once
#include <iostream>
#include <type_traits>
namespace helper
{
	template <typename T>
	struct CallbackInfoHelper;

	template <typename R, typename... Args>
	struct CallbackInfoHelper<R(Args...)> {
		using ParamMiddleType = std::tuple<typename std::type_identity<Args>::type...>;
		using retType = R;
		using functionType = R(Args...);
		template<size_t N>
		using ParamType = typename std::tuple_element<N, ParamMiddleType>::type;
	};
	template <typename R, typename... Args>
	struct CallbackInfoHelper<R(*)(Args...)> {
		using ParamMiddleType = std::tuple<typename std::type_identity<Args>::type...>;
		using retType = R;
		using functionType = R(Args...);
		template<size_t N>
		using ParamType = typename std::tuple_element<N, ParamMiddleType>::type;
	};

	/*
	* 需要使用decltype(&claname::function)来获取类型
	*/
	template <typename ClassName,typename R, typename... Args>
	struct CallbackInfoHelper<R(ClassName::*)(Args...)> {
		using ParamMiddleType = std::tuple<typename std::type_identity<Args>::type...>;
		using retType = R;
		using functionType = R(Args...);
		template<size_t N>
		using ParamType = typename std::tuple_element<N, ParamMiddleType>::type;
	};
	 
	//因为也可能存在R(* const)(Args...)、R(* &)(Args...)这些一次给他退化了
	template<typename T>
	using CallbackInfo = CallbackInfoHelper<std::decay_t<T>>;

 
	template<std::uintptr_t>
	struct CallBackStore {};


	template<typename T>
	constexpr auto TakeCallBackStore(T function)
	{
		constexpr static CallBackStore<T, function> v{};
		return v;
	}

    template<typename Callback>
	static constexpr auto getType(Callback callback)
	{
		constexpr static Callback v{};
		return v;
	}
}

/*
* void hello(int p)
{

}

int hellop(int q,float b)
{
	return 0;
}

//constexpr auto valkue = std::is_function_v<decltype(hello)>;
using type1 = helper::CallbackInfo<decltype(hello)>::retType;
using type2 = decltype(helper::getType(hello));
using ty = decltype(hello);
using type3 = helper::CallbackInfo<decltype(hellop)>::ParamType<0>;
using type4 = helper::CallbackInfo<decltype(hellop)>::ParamType<1>;
using type5 = float*;
constexpr static auto same = std::is_same_v<float, type4>;
*/