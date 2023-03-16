#pragma once
#include <bitset>
#include <boost/hana.hpp>
#include <concept/traits/tuple_trait.hpp>
/*
* 将多个bitset合并成一个大的
* auto bits = helper::bitsetCombine(std::bitset<6>(0x01), std::bitset<6>(0x0f));
	std::cout << bits << '\n';
*/

namespace helper 
{

	using bitLengthType = unsigned long;
	/*
	* 多个bitset整合成一个
	*/
	template<bitLengthType...N>
	auto bitsetCombine(std::tuple<std::bitset<N>...> bits)
	{
		constexpr static bitLengthType totalBitSize = boost::hana::sum<bitLengthType>(boost::hana::make_tuple(N...));
		using retType = std::bitset<totalBitSize>;
		auto bitstr = std::apply([](auto&&... args)
			{
				std::string bitString;
				bitString = (args.to_string() + ...);
				return std::move(bitString);
			}, bits);
		retType ret(bitstr);
		return std::move(ret);
	}

	template<bitLengthType...N>
	inline auto bitsetCombine(std::bitset<N>... bits)
	{
		return std::move(bitsetCombine(std::make_tuple(bits...)));
	}

 
	/*
	* 一个bitset分解成多个
	* TEST(bitsetTest, scene2)
{
	//GTEST_SKIP();
	auto bits = helper::bitsetCombine(std::bitset<6>(0x01), std::bitset<6>(0x0f));
	std::cout << bits << '\n';
	auto tuple = helper::bitsetSplit<6,6>(bits);
	traits::tuple_for_each([](auto& t) {
		std::cout << t << '\n';
		}, tuple);
}

	*/
	template<bitLengthType...N>
	std::tuple<std::bitset<N>...> bitsetSplit(std::bitset<boost::hana::sum<bitLengthType>(boost::hana::make_tuple(N...))> completeBit)
	{
		std::tuple<std::bitset<N>...> ret;
		std::string completeString = completeBit.to_string();
		int offset = 0;
		traits::tuple_for_each([&]<typename tupleType>(tupleType& tuple)
			{
			auto size = tuple.size();
				std::string bitString = completeString.substr(offset, size);
				tupleType tuple_buf(bitString);
				tuple = tuple_buf;
				offset += size;
			}, ret);
	
		return std::move(ret);

	}

}