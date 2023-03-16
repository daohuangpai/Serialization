#pragma once
#include <boost/hana.hpp>
#include <concept/traits/traits.hpp>
namespace traits
{
	//从定义BOOST_HANA_DEFINE_STRUCT_IMPL_1中可以看出如果是反射类那么就会有一个hana_accessors_impl的内部函数
	template<typename T>
	struct is_hana_reflection_struct :public std::conditional_t < requires(T v) { typename T::hana_accessors_impl; }, std::true_type, std::false_type > {};

	//template<typename T>
	//concept hana_reflection_struct = is_hana_reflection_struct<T>::value;
	//template<typename T>
	//concept normalContain =  
	//  requires (T v)
	//{
	//	v.begin();
	//	v.end();
	//}
	//;

	/*
	* 所有each_element的遍历的值一律支持使用引用，这样可以在回调中改变这个值
	*/
	template<typename contianer, typename Func>
	requires (requires (contianer v)
	{
		v.begin();
		v.end();
	})
	static void each_element(contianer& cs, Func f)
	{
			for (auto& itr : cs)
			{
				f(itr);
			}
	}

	template<typename contianer, typename Func>
	requires (is_hana_reflection_struct<contianer>::value)
	static void each_element(contianer& cs, Func f)
	{
			//反射类的访问器
			static constexpr auto accessors = boost::hana::accessors<contianer>();
			//所有反射元素的数量
			static constexpr auto accessorsSize = boost::hana::size(accessors);

			boost::hana::for_each(boost::hana::make_range(boost::hana::int_c<0>, boost::hana::int_c<accessorsSize>), [&](auto x) {
				constexpr static auto keyName = boost::hana::first(accessors[boost::hana::size_c<x>]).c_str();
				f(keyName, boost::hana::second(accessors[boost::hana::size_c<x>])(cs));
				});
	}
    
	/*
	* 		auto tuple = std::make_tuple(1, 2, 3);
		traits::each_element(tuple, [](auto& v) {
			std::cout << v << std::endl;
		v = 0;
			});
			执行完后全是0
	*/
	template<typename contianer, typename Func>
	requires (is_template_of_v<std::tuple,contianer>)
	static void each_element(contianer& cs, Func f)
	{
		std::apply([&](auto&&... args) {
			(f(args), ...);
			}, cs);
	}

}