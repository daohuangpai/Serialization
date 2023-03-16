#pragma once
#include <boost/hana.hpp>
#include <concept/traits/traits.hpp>
namespace traits
{
	//�Ӷ���BOOST_HANA_DEFINE_STRUCT_IMPL_1�п��Կ�������Ƿ�������ô�ͻ���һ��hana_accessors_impl���ڲ�����
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
	* ����each_element�ı�����ֵһ��֧��ʹ�����ã����������ڻص��иı����ֵ
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
			//������ķ�����
			static constexpr auto accessors = boost::hana::accessors<contianer>();
			//���з���Ԫ�ص�����
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
			ִ�����ȫ��0
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