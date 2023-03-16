// �ļ�����TestReflecttorEncryption
// ����ʱ�䣺2023/3/16 11:15:00
#define test_TestReflecttorEncryption
#ifdef test_TestReflecttorEncryption
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include <concept/encryption/reflectorDataEncry.hpp>

//����һ��������
struct ReflectorClass {
    __reflectorWithSerialize(
        (uint32)idInt,
        (float)valueFloat,
        (double)valueDouble,
        (std::string)valueString
    )

     __reflectorSerializeEquation(ReflectorClass);//����Ⱥ�
};

TEST(TestReflecttorEncryption, Reflect)
{
    ReflectorClass rawValue = { 1,2.678,98.3849756,"hello world!" };
    std::cout << "visit reflectorClass:" << std::endl;
    Reflector::visit_each(rawValue, [](auto fieldData)
        {
            std::cout << fieldData.name() << ":" << fieldData.get() << std::endl;
        });
}


/*
* �Է�����ı���ͽ���
*/
TEST(TestReflecttorEncryption, Reflecttor)
{
        ReflectorClass rawValue = { 1,2.678,98.3849756,"hello world!" };
         
        {
            size_t readbyte;
            auto encodeString = encryption::ReflectorDataJsonEncry<ReflectorClass, std::uint64_t, encryption::BIG_ENDIAN>::Encode<std::vector<unsigned char>>(rawValue);
            auto decodereflectorClass = encryption::ReflectorDataJsonEncry<ReflectorClass, std::uint64_t, encryption::BIG_ENDIAN>::Decode(encodeString.begin(), encodeString.end(), &readbyte);
            EXPECT_EQ(readbyte, std::size(encodeString));
            EXPECT_EQ(rawValue, decodereflectorClass);
            std::cout << Reflector::to_string(decodereflectorClass) << std::endl;
        }
        {
            size_t readbyte;
            auto encodeString = encryption::ReflectorDataJsonEncry<ReflectorClass, std::uint64_t, encryption::NATIVE>::Encode<std::vector<unsigned char>>(rawValue);
            auto decodereflectorClass = encryption::ReflectorDataJsonEncry<ReflectorClass, std::uint64_t, encryption::NATIVE>::Decode(encodeString.begin(), encodeString.end(), &readbyte);
            EXPECT_EQ(readbyte, std::size(encodeString));
            EXPECT_EQ(rawValue, decodereflectorClass);
            std::cout << Reflector::to_string(decodereflectorClass) << std::endl;
        }
        {
            size_t readbyte;
            auto encodeString = encryption::ReflectorDataJsonEncry<ReflectorClass, std::uint64_t, encryption::LITTLE_ENDIAN>::Encode<std::vector<unsigned char>>(rawValue);
            auto decodereflectorClass = encryption::ReflectorDataJsonEncry<ReflectorClass, std::uint64_t, encryption::LITTLE_ENDIAN>::Decode(encodeString.begin(), encodeString.end(), &readbyte);
            EXPECT_EQ(readbyte, std::size(encodeString));
            EXPECT_EQ(rawValue, decodereflectorClass);
            std::cout << Reflector::to_string(decodereflectorClass) << std::endl;
        }
}

#endif