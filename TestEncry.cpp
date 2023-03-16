// 文件名：TestEncry
// 创建时间：2023/3/16 10:07:13
#define test_TestEncry
#ifdef test_TestEncry
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include <concept\encryption\encry>
#include <concept/helper/binDebug.hpp>
extern "C"
{
#include <openssl/applink.c>
};

TEST(TestNormal, scene1)
{
	/*
* 编解码一个std::string类型为一个
  0xff 小端的std::uint64_t（描述字符串长度） std::string 0x0f, 0xff, 0x0f
  这样结构的数据流，然后可以传输和使用Decode反解码成std::string
*/
using encodeType = encryption::BackFixEncry<
	encryption::HeadFixEncry<
	encryption::bufferEncry<std::string, std::uint64_t, encryption::LITTLE_ENDIAN>,
	0xff>,
	0x0f, 0xff, 0x0f>;


	GTEST_SKIP();
	std::string text = "hello world";
	auto encodeData = encodeType::Encode<std::string>(text);
	helper::dump(encodeData);
	/*helper::dump(encodeData)输出可以看到0xff开头、0x0f, 0xff, 0x0f结尾中间使用小端存储string text的长度（uint64_t），然后是字符串"hello world"
	* 0000 - ff 0b 00 00 00 00 00 00-00 68 65 6c 6c 6f 20 77   .........hello w
	  0010 - 6f 72 6c 64 0f ff 0f                  orld...
	  hello world
	*/
	size_t byteRead;
	auto decodetext = encodeType::Decode(std::begin(encodeData), std::end(encodeData), &byteRead);
	EXPECT_EQ(byteRead, std::size(encodeData)) << "error";
	EXPECT_EQ(decodetext, text) << "error";
	std::cout << decodetext << std::endl;

}


struct typeEncryClass
{
	float fvalue;
	int ivalue;
	bool operator==(const typeEncryClass& other)const
	{
		return ((fvalue == other.fvalue) && (ivalue == other.ivalue));
	}
};

struct ReflectorClass {
	__reflectorWithSerialize(
		(uint32)idInt,
		(float)valueFloat,
		(double)valueDouble,
		(std::string)valueString
	)

		__reflectorSerializeEquation(ReflectorClass);//定义等号
};

TEST(TestNormal, scene2)
{
	/*
* 编解码一个std::string类型为一个
  0xff 小端的std::uint64_t（描述字符串长度） std::string 0x0f, 0xff, 0x0f
  这样结构的数据流，然后可以传输和使用Decode反解码成std::string
*/
	using encodeType = encryption::BackFixEncry<
		encryption::HeadFixEncry<
	    encryption::linkEncryptionEncry<
			encryption::bufferEncry<std::string, std::uint64_t, encryption::LITTLE_ENDIAN>,
			encryption::typeEncry<typeEncryClass>,
			encryption::ReflectorDataJsonEncry<ReflectorClass>
		>,
		0xff>,
		0x0f, 0xff, 0x0f>;

	std::string text = "hello world";
	typeEncryClass typeEncry = {1.98,9};
	ReflectorClass rawValue = { 1,2.678,98.3849756,"hello world!" };
	auto value = std::make_tuple(text, typeEncry, rawValue);
	auto encodeData = encodeType::Encode<std::vector<char>>(value);
	helper::dump(encodeData);
	size_t byteRead;
	//解码结果也是一个和value一样的tuple结构
	auto decodevalue = encodeType::Decode(std::begin(encodeData), std::end(encodeData), &byteRead);
	EXPECT_EQ(byteRead, std::size(encodeData)) << "error";

	EXPECT_EQ(value, decodevalue);
}


#endif