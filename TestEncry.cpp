// �ļ�����TestEncry
// ����ʱ�䣺2023/3/16 10:07:13
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
* �����һ��std::string����Ϊһ��
  0xff С�˵�std::uint64_t�������ַ������ȣ� std::string 0x0f, 0xff, 0x0f
  �����ṹ����������Ȼ����Դ����ʹ��Decode�������std::string
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
	/*helper::dump(encodeData)������Կ���0xff��ͷ��0x0f, 0xff, 0x0f��β�м�ʹ��С�˴洢string text�ĳ��ȣ�uint64_t����Ȼ�����ַ���"hello world"
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

		__reflectorSerializeEquation(ReflectorClass);//����Ⱥ�
};

TEST(TestNormal, scene2)
{
	/*
* �����һ��std::string����Ϊһ��
  0xff С�˵�std::uint64_t�������ַ������ȣ� std::string 0x0f, 0xff, 0x0f
  �����ṹ����������Ȼ����Դ����ʹ��Decode�������std::string
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
	//������Ҳ��һ����valueһ����tuple�ṹ
	auto decodevalue = encodeType::Decode(std::begin(encodeData), std::end(encodeData), &byteRead);
	EXPECT_EQ(byteRead, std::size(encodeData)) << "error";

	EXPECT_EQ(value, decodevalue);
}


#endif