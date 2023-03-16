#pragma once
#include <tuple>
#include <concept/traits/traits.hpp>
#include <type_traits>
#include <optional>
#include <concept/traits/is_any_of.hpp>
namespace traits
{
	/*
	* 遍历全部的tuple
	* 	auto tuple = std::make_tuple(1, "hello", 8.0f);
	    traits::tuple_for_each(
		[](auto& value)
		{
			std::cout << typeid(value).name() <<":"<<value<< std::endl;

		},tuple);
       输出:
			int:1
			char const * __ptr64:hello
			float:8

	*/
	template<typename T>
	concept tupleType = is_template_of_v<std::tuple, T>;

	template<typename functionType, tupleType tuple_type,typename T, T... ints>
	void tuple_for_each_impl(tuple_type& tuple, functionType fuc,std::integer_sequence<T, ints...>)
	{
		/*static_assert(is_template_of_v<std::tuple, tuple_type>,"need tuple");*/
		((fuc(std::get<ints>(tuple))), ...);
	}

	template<typename functionType,typename...Args>
	inline void tuple_for_each(functionType func, std::tuple<Args...>& tuple)
	{
		tuple_for_each_impl(tuple, func, std::make_integer_sequence<unsigned int, std::tuple_size<std::tuple<Args...>>::value>{});
	}

	/*
	* 提取第N个元素到最后一个元素组成一个新的tuple
	* std::tuple<int, double, std::string,int> t1{ 1, 2.3, "hello" ,6};
     从t1中提取第二个元素以及剩余元素
	auto t2 = take_new_tuple_from<1>(t1);
	std::cout << std::get<0>(t2) << ", " << std::get<1>(t2) << std::endl;
	*/
	template <std::size_t N, typename Tuple, typename... Types>
	auto take_new_tuple_from(Tuple&& tuple, Types&&... args) {
		static constexpr auto endnum = std::tuple_size_v<std::decay_t<Tuple>> -1;
		if constexpr (N == endnum) {
			return std::make_tuple(
				std::forward<Types>(args)..., std::forward<decltype(std::get<N>(tuple))>(std::get<N>(tuple)));
		}
		else {
			return take_new_tuple_from<N + 1>(std::forward<Tuple>(tuple),
				std::forward<Types>(args)...,
				std::forward<decltype(std::get<N>(tuple))>(std::get<N>(tuple)));
		}
	}

	template<typename Tuple>
	struct tupleRef;
	template<typename... TupleArgs>
	struct tupleRef<std::tuple<TupleArgs...>>
	{
		using type = std::tuple<std::add_lvalue_reference_t<TupleArgs>...>;
	};

	template<typename Tuple>
	struct tuplePointer;
	template<typename... TupleArgs>
	struct tuplePointer<std::tuple<TupleArgs...>>
	{
		using type = std::tuple<std::add_pointer_t<TupleArgs>...>;
	};


	template<typename Tuple1, typename Tuple2>
	struct TupleCombine;
	template<typename... TupleArgs1, typename... TupleArgs2>
	struct TupleCombine<std::tuple<TupleArgs1...>, std::tuple<TupleArgs2...>>
	{
		using type = std::tuple<TupleArgs1..., TupleArgs2...>;
	};
	template<typename Tuple>
	struct tupleOptional;
	template<typename... TupleArgs>
	struct tupleOptional<std::tuple<TupleArgs...>>
	{
		using type = std::tuple<std::optional<TupleArgs>...>;
	};

	//判断ThisType是不是tuple中的一员
	template<typename ThisType,typename tupleType>
	struct is_any_of_tuple;
	

	template<typename ThisType, typename... tupleTypes>
	struct is_any_of_tuple<ThisType, std::tuple<tupleTypes...>>
		:public is_any_of<ThisType, tupleTypes...> {};


}