#define test_TestNetEncryption
#ifdef test_TestNetEncryption
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "concept/encryption/network/netLengthEncry.hpp"
#include "concept/encryption/network/bitLengthEncry.hpp"
#include <concept/encryption/network/address_ip_encry.hpp>
#include <concept/encryption/network/sock5Encry.hpp>
#include <concept/helper/binDebug.hpp>
/*
* 测试网络长度数据的编解码，网络数据的长度的位排序（大小端）和本机有时候会是不同的，使用netLengthEncry直接使用类型即可
*/
TEST(TestEncryption, netLengthEncry)
{
	using shortEncode = encryption::netLengthEncry<unsigned short>;
	using longEncode = encryption::netLengthEncry<unsigned long>;

	{
		GTEST_SKIP();
		unsigned short sNum = 100;
		auto buffer = shortEncode::Encode<std::vector<std::uint8_t>>(sNum);

		unsigned short encodeTo = 0;
		//std::copy(std::begin(buffer), std::end(buffer), &encodeTo);
		memcpy(&encodeTo, buffer.data(), buffer.size());
		std::cout << "org:" << std::hex << sNum << "	encodeTo" << std::hex << encodeTo << std::endl;;

		size_t readbyte = 0;
		auto decodeSNum = shortEncode::Decode(std::begin(buffer), std::end(buffer), &readbyte);
		EXPECT_EQ(readbyte, sizeof(sNum)) << "shortEncode number error!";
		EXPECT_EQ(sNum, decodeSNum) << "shortEncode error!";
	}
	{
		unsigned long sNum = 100;
		auto buffer = longEncode::Encode<std::vector<std::uint8_t>>(sNum);

		unsigned long encodeTo = 0;
		//std::copy(std::begin(buffer), std::end(buffer), &encodeTo);
		memcpy(&encodeTo, buffer.data(), buffer.size());
		std::cout << "org:" << std::hex << sNum << "	encodeTo:" << std::hex << encodeTo << std::endl;;

		size_t readbyte = 0;
		auto decodeSNum = longEncode::Decode(std::begin(buffer), std::end(buffer), &readbyte);
		EXPECT_EQ(readbyte, sizeof(sNum)) << "longEncode number error!";
		EXPECT_EQ(sNum, decodeSNum) << "longEncode error!";
	}
}



//constexpr static 

template<unsigned int...N>
void testbitLengthEncry(std::bitset<N>... bits)
{
	GTEST_SKIP();
	static_assert(sizeof...(bits) > 0, "error");
	auto tu = std::make_tuple(bits...);
	//编码成网络流，buffer就是编码成的网络数据流std::vector<std::uint8_t>
	auto buffer = encryption::bitLengthEncry<N...>::template Encode<std::vector<std::uint8_t>>(tu);
	size_t byteread = 0;
	//解码的数据流，将buffer重新解码成多个bits，只不过输入时多个std::bitset<N>... bits，这里是std::tuple<std::bitset<N>...>
	auto tupleBits = encryption::bitLengthEncry<N...>::template Decode(std::begin(buffer), std::end(buffer), &byteread);
	EXPECT_EQ(byteread, std::size(buffer)) << "byte read error!";
	std::cout << "before decode:     ";
	((std::cout << bits << ","), ...);
	std::cout << std::endl;
	std::cout << "decode:            ";
	std::apply([&](auto&&... tus) {
		//((EXPECT_EQ(tus, bits)<< "decode error!"),...);
		((std::cout << tus << ","), ...);
		}, tupleBits);
	std::cout << std::endl;
	EXPECT_EQ(tu, tupleBits) << "tuple decode error!";

}

/*
* 网络协议中经常有将一个char或者int的几个位单独拿出来作为一个类型使用
* 比如char就可以有 3、2、3这种分配形式就可以使用
* encryption::bitLengthEncry<3, 2, 3>;
* 则可以使用
* encryption::bitLengthEncry<3, 2, 3>::Encode<std::vector<std::uint8_t>>(char)将数据编码成std::vector<std::uint8_t>
* 并使用
* encryption::bitLengthEncry<3, 2, 3>::Decode<std::vector<std::uint8_t>>(std::begin(buffer), std::end(buffer), &byteread)解码数据为
* std::tuple<std::bitset<3>,std::bitset<2>,std::bitset<3>>
*/
TEST(testAsioNetProtool, bitLengthEncry)
{
	GTEST_SKIP();
	using bitLengthEncry = encryption::bitLengthEncry<3, 2, 3>;

	std::cout << bitLengthEncry::totalBitSize << std::endl;
	std::cout << typeid(bitLengthEncry::encodeType).name() << std::endl;
	std::cout << typeid(bitLengthEncry::EncodeNumberTypeHelper).name() << std::endl;
	std::cout << typeid(bitLengthEncry::EncodeNumberType).name() << std::endl;

	//要求参数所有的bit位加起来是一个整型数据的bit长度，因为是对网络的bit数据进行编解码
	testbitLengthEncry(std::bitset<9>(0x01), std::bitset<9>(0x0), std::bitset<14>(0x02), std::bitset<9>(0x01), std::bitset<9>(0x0), std::bitset<14>(0x02));
	testbitLengthEncry(std::bitset<5>(0x01), std::bitset<4>(0x0), std::bitset<9>(0x0), std::bitset<7>(0x02), std::bitset<7>(0x34), std::bitset<9>(0x01), std::bitset<9>(0x0), std::bitset<14>(0x02));
}

/*
* 使用asio的结构对ip地址进行编码解码
*/
TEST(testipEntry, ip)
{
	GTEST_SKIP();
	{
		auto ip4 = asio::ip::make_address_v4("192.168.10.5");
		auto buffer = encryption::Ip4_Encry::Encode<std::vector<std::uint8_t>>(ip4);
		size_t readByte;
		auto ip4Decode = encryption::Ip4_Encry::Decode(std::begin(buffer), std::end(buffer), &readByte);
		EXPECT_EQ(buffer.size(), readByte) << "error";
		EXPECT_EQ(ip4, ip4Decode) << "error";

		std::cout << ip4 << std::endl;
		std::cout << ip4Decode << std::endl;
	}
	{
		auto ip6 = asio::ip::make_address_v6("2001:0db8:3c4d:0015:0000:0000:1a2f:1a2b");
		auto buffer = encryption::Ip6_Encry::Encode<std::vector<std::uint8_t>>(ip6);
		size_t readByte;
		auto ip6Decode = encryption::Ip6_Encry::Decode(std::begin(buffer), std::end(buffer), &readByte);
		EXPECT_EQ(buffer.size(), readByte) << "error";
		EXPECT_EQ(ip6, ip6Decode) << "error";

		std::cout << ip6 << std::endl;
		std::cout << ip6Decode << std::endl;
	}

}

/*
* sock5是一个服务器经常用到的代理协议
* 认证方法
*/
enum method : std::uint8_t
{
	NO_AUTHENTICATION_REQUIRED = 0x00,
	GSSAPI = 0x01,
	USERNAME_PASSWORD = 0x02,
	//X’03’ to X’7F’ IANA ASSIGNED （ 由IANA分配（保留）
	//X’80’ to X’FE’ RESERVED FOR PRIVATE METHODS 私人方法保留）
	NO_ACCEPTABLE_METHODS = 0xFF
};
TEST(sock5, HandShockChuckType)
{
	GTEST_SKIP();
	//只需要写认证方法然后放入HandShockChuckType::Encode即可生成sock5客户请求头
	std::vector<unsigned char> data = { NO_AUTHENTICATION_REQUIRED,GSSAPI,USERNAME_PASSWORD };
	auto dataBuffer = HandShockChuckType::Encode<std::vector<unsigned char>>(data);
	std::cout << "sock5 HandShockChuckType:" << std::endl;
	helper::dump(dataBuffer);

	size_t byte;
	auto DecodedataBuffer = HandShockChuckType::Decode(dataBuffer.begin(), dataBuffer.end(), &byte);
	//验证解码成功
	EXPECT_EQ(DecodedataBuffer, data) << "error";

}
TEST(sock5, HandShockReplyChuckType)
{
	GTEST_SKIP();
	unsigned char data = 0;
	auto dataBuffer = HandShockReplyChuckType::Encode<std::vector<unsigned char>>(data);
	std::vector<unsigned char> ExceptData = { 0x05 ,0x00 };
	EXPECT_EQ(dataBuffer, ExceptData) << "error";
	std::cout << "sock5 HandShockReplyChuckType:" << std::endl;
	helper::dump(dataBuffer);

	size_t byte;
	auto DecodedataBuffer = HandShockReplyChuckType::Decode(dataBuffer.begin(), dataBuffer.end(), &byte);
	EXPECT_EQ(DecodedataBuffer, data) << "error";

}
/*
* sock5可以使用ip4、ip6以及网址host名称返回需要连接的目标给服务器，下面是三种情况，但是接口都是统一的
*/
TEST(sock5, Sock5RequestIp4)
{
	GTEST_SKIP();
	unsigned char cmd = 1;
	asio::ip::address add = asio::ip::address::from_string("127.0.0.1");
	Sock6Addr address = add;
	unsigned short port = 10086;
	auto tu = std::make_tuple(cmd, Sock6Addr(add), port);
	auto buffer = Sock5Request::Encode<std::vector<unsigned char>>(tu);
	std::cout << "sock5 Sock5RequestIp4:" << std::endl;
	helper::dump(buffer);
	size_t readbyte;
	auto tus = Sock5Request::Decode(std::begin(buffer), std::end(buffer), &readbyte);
	EXPECT_EQ(readbyte, std::size(buffer)) << "error";
	EXPECT_EQ(tu, tus) << "error";

}

TEST(sock5, Sock5RequestIp6)
{
	GTEST_SKIP();
	unsigned char cmd = 1;
	asio::ip::address add = asio::ip::address::from_string("1002:003B:456C:678D:890E:0012:234F:5657");
	Sock6Addr address = add;
	unsigned short port = 10086;
	auto tu = std::make_tuple(cmd, address, port);
	auto buffer = Sock5Request::Encode<std::vector<unsigned char>>(tu);
	std::cout << "sock5 Sock5RequestIp6:" << std::endl;
	helper::dump(buffer);
	size_t readbyte;
	auto tus = Sock5Request::Decode(std::begin(buffer), std::end(buffer), &readbyte);
	EXPECT_EQ(readbyte, std::size(buffer)) << "error";
	EXPECT_EQ(tu, tus) << "error";

}
TEST(sock5, Sock5RequestHostName)
{
	GTEST_SKIP();
	unsigned char cmd = 1;
	Sock6Addr address = "cn.bing.com";
	unsigned short port = 10086;
	auto tu = std::make_tuple(cmd, address, port);
	auto buffer = Sock5Request::Encode<std::vector<unsigned char>>(tu);
	std::cout << "sock5 Sock5RequestHostName:" << std::endl;
	helper::dump(buffer);
	size_t readbyte;
	auto tus = Sock5Request::Decode(std::begin(buffer), std::end(buffer), &readbyte);
	EXPECT_EQ(readbyte, std::size(buffer)) << "error";
	EXPECT_EQ(tu, tus) << "error";

}
#endif