#pragma once
#include "concept/encryption/typeEncry.hpp"
#include <boost/hana.hpp>
#include <concept/helper/stlHelper/bitset.hpp>
#include <boost/endian.hpp>
#include <type_traits>
/*
* 
* 仅用于网络传输的数据封装使用，将任意一个长度转为bit类型
* 
* 用于一组bit不按照bit大小分割成多块用于描述信息的
* 比如ip协议头
*    0                   1                   2                   3   
    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |Version|  IHL  |Type of Service|          Total Length         |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |         Identification        |Flags|      Fragment Offset    |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |  Time to Live |    Protocol   |         Header Checksum       |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |                       Source Address                          |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |                    Destination Address                        |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |                    Options                    |    Padding    |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   Version使用1-3 4个bit
   IHL 4-7 4个bit
   那么前两个就可以使用
   encryption::bitLengthEncry<4，4>描述
   encryption::bitLengthEncry<4，4>::encodeType = std::tuple<std::bitset<4>,std::bitset<4>>
* 
* 
* 
* 
* 
* 
* 
* 
* 
* 
* 
* 
* 
* template<unsigned int...N>
void testbitLengthEncry(std::bitset<N>... bits)
{
	static_assert(sizeof...(bits) > 0, "error");
	auto tu = std::make_tuple(bits...);
	auto buffer = encryption::bitLengthEncry<N...>::template Encode<std::vector<std::uint8_t>>(tu);
	 
	std::cout << "encode by function:";
	for (auto itr : buffer) {
		std::bitset<8> n(itr);
		std::cout << n;
	}
	std::cout << std::endl;

	std::cout << "encode by string:  ";
	//ntohl
	std::apply([](std::bitset<N>... bits) {
		((std::cout << bits), ...);
		}, tu);
	std::cout << std::endl;

	size_t byteread = 0;
	auto tupleBits = encryption::bitLengthEncry<N...>::template Decode(std::begin(buffer), std::end(buffer), &byteread);
	EXPECT_EQ(byteread, std::size(buffer)) << "byte read error!";
	std::cout << "before decode:     ";
	((std::cout << bits << ","), ...);
	std::cout << std::endl;
	std::cout << "decode:            ";
	std::apply([&](auto&&... tus) {
		//((EXPECT_EQ(tus, bits)<< "decode error!"),...);
		((std::cout << tus <<","), ...);
		}, tupleBits);
	std::cout << std::endl;
	EXPECT_EQ(tu, tupleBits) << "tuple decode error!";

}

TEST(bitLengthEncry, scene1)
{
testbitLengthEncry(std::bitset<9>(0x01), std::bitset<9>(0x0), std::bitset<14>(0x02), std::bitset<9>(0x01), std::bitset<9>(0x0), std::bitset<14>(0x02));
}
*/
NAMESPACE_BIN_ENCRY
{
	using bitLengthEncryType = unsigned long;

   

	template<bitLengthEncryType... bitSizes>
	struct bitLengthEncry
	{
		//多个bitset总的bit数
		constexpr static bitLengthEncryType totalBitSize = boost::hana::sum<bitLengthEncryType>(boost::hana::make_tuple(bitSizes...));
		constexpr static bitLengthEncryType bitDivisibility = 8;
		//多个bitset总的字节数
		constexpr static bitLengthEncryType byteNeed = totalBitSize / bitDivisibility;
		//多个bitset总的bit数必须是计算机中存在的某种长度类型的具体大小
		static_assert(byteNeed * bitDivisibility == totalBitSize, "totalBitSize require be divisibility by bitDivisibility");
		using encodeType = std::tuple<std::bitset<bitSizes>...>;

		using ulongType = decltype(std::declval<std::bitset<8>>().to_ulong());
		using ullongType = decltype(std::declval<std::bitset<8>>().to_ullong());
		//使用的用来描述多个bitset合并成的或者说御用描述其的一个类型
		constexpr static auto EncodeNumberTypeHelper = 
			boost::hana::unpack(boost::hana::make_tuple(0), [&](auto...) {

				if constexpr (byteNeed == sizeof(std::uint8_t)) {
					return boost::hana::type_c<std::uint8_t>;
				}
				else if constexpr (byteNeed == sizeof(unsigned short)) {
					return boost::hana::type_c<unsigned short>;
				}
				else if constexpr (byteNeed == sizeof(ulongType)) {
					return boost::hana::type_c<ulongType>;
				}
				else if constexpr (byteNeed == sizeof(ullongType)) {
					return boost::hana::type_c<ullongType>;
				}
				}
		);
		//因为直接调用会出现cl.exe错误所以加了个EncodeNumberTypeHelper1
		using EncodeNumberTypeHelper1 = decltype(EncodeNumberTypeHelper);
		//这个是编码的长度的类型，只能有上面四种
		using EncodeNumberType = EncodeNumberTypeHelper1::type;


		template<typename EncodeContainerType>
		static EncodeContainerType Encode(encodeType& data)
		{
			std::uint8_t buffer[byteNeed];
			std::bitset<totalBitSize> bitBuffer = helper::bitsetCombine(data);

			//if constexpr (byteNeed < sizeof(ullongType)) {
			//	EncodeNumberType* u = reinterpret_cast<EncodeNumberType*>(&buffer[0]);
			//	*u = bitBuffer.to_ulong();
			//}
			//else if constexpr (byteNeed == sizeof(ullongType)) {
			//	EncodeNumberType* u = reinterpret_cast<EncodeNumberType*>(&buffer[0]);
			//	*u = bitBuffer.to_ullong();
			//}
			//这里记得本机转大端，因为网络数据是大端模式，bitset的to_ulong不会输出大端模式的数据
			static_assert(byteNeed<= sizeof(ullongType), "error byte size,only support length type");
			if constexpr (byteNeed < sizeof(ullongType)) {
				EncodeNumberType u = bitBuffer.to_ulong();
				//u = ::htonl(u);
				u = boost::endian::native_to_big(u);
				::memcpy_s(buffer, byteNeed, &u, sizeof(u));
			}
			else if constexpr (byteNeed == sizeof(ullongType)) {
				EncodeNumberType u = bitBuffer.to_ullong();
				//u = ::htonll(u);
				u = boost::endian::native_to_big(u);
				::memcpy_s(buffer, byteNeed, &u, sizeof(u));
			}

	/*		else {
				static_assert(false, "error byte size,only support length type");
			}*/
			EncodeContainerType ret(std::begin(buffer), std::end(buffer));
			return std::move(ret);
			//bitBuffer.
		}

		template<typename iterator>
		static encodeType Decode(iterator begin, iterator end, size_t* readbyte)
		{
			__error_throw_out_UserErrorCode(byteNeed <= end - begin, DataProtocolStatue::LACK_DATA, "buffer too small")
			unsigned char buffer[byteNeed];
			std::copy(begin, end, std::begin(buffer));
			*readbyte = byteNeed;
			EncodeNumberType u;
			static_assert(sizeof(EncodeNumberType) == byteNeed,"data size error!");
			::memcpy_s(&u, sizeof(u), buffer, byteNeed);
			u = boost::endian::big_to_native(u);
			std::bitset<totalBitSize> bitBuffer = { u };
			return std::move(helper::bitsetSplit<bitSizes...>(bitBuffer));
		}
	};
}

