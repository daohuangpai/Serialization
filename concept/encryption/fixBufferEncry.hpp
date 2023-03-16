#pragma once

#include "concept/encryption/encryHead.hpp"
NAMESPACE_BIN_ENCRY
{
	/*
	* �ڱ�����Encry��ǰ�����һ���̶����ַ���
	* �������������ǰ�����1��2��3��������ֻ��һ�����
	* ��Ϊ
	* using encodeType = Encry::encodeType;
	* �õı��������Ҳ���������е�
	* 
	* 
	* Ҫע�������fixElement��unsigned char�����ʹ�ó�������ṹ�����������ֲ���Ԥ֪������
	* struct binpi {
    __reflectorWithSerialize(
       (uint32)p,
       (float)yu
    )
    
};
	* #include "concept/encryption/fixBufferEncry.hpp"

int main()
{
    binpi v = { 1,2.678 };
    size_t readbyte = 0;
    auto buffer = encryption::HeadFixEncry<encryption::typeEncry<binpi>, 1, 2, 3>::Encode<std::vector<unsigned char>>(v);
    auto value = encryption::HeadFixEncry<encryption::typeEncry<binpi>, 1, 2, 3>::Decode(buffer.begin(), buffer.end(),&readbyte);
    std::cout << std::to_string(value) << std::endl;
    return 0;
}
	*/
	template<ConceptEncryption Encry,unsigned char...fixElement>
	struct HeadFixEncry
	{
		 static constexpr std::array<unsigned char,sizeof...(fixElement)> fixElementArray = {fixElement...};
		 __SIGNED_ENCRYPTION
		 using encodeType = Encry::encodeType;
		 template<typename EncodeContainerType>
		 static EncodeContainerType Encode(encodeType& data)
		 {
			 auto EncryData = Encry::template Encode<EncodeContainerType>(data);
			 EncryData.insert(std::begin(EncryData),std::begin(fixElementArray),std::end(fixElementArray));
			 return std::move(EncryData);
		 }

		 template<typename iterator>
		 static encodeType Decode(iterator begin, iterator end, size_t* readbyte)
		 {
			 size_t dataSize = end - begin;
			 __error_throw_out_UserErrorCode(dataSize >= fixElementArray.size(), DataProtocolStatue::LACK_DATA, "data error")

			 for (auto& item : fixElementArray) {
				 __error_throw_out_UserErrorCode(item == static_cast<unsigned char>(*begin), DataProtocolStatue::ERROR_DATA, "data error")
				 begin++;
			 }
			 size_t n = 0;
			 auto ret = Encry::template Decode(begin, end, &n);
			 if (readbyte)
				 *readbyte = n + fixElementArray.size();
			 return std::move(ret);
		 }
    };

	/*
	* �����湦�����ƣ�ֻ�ǰѹ̶������ݼ���β��
	* struct binpi {
    __reflectorWithSerialize(
       (uint32)p,
       (float)yu
    )
    
};
#include "concept/encryption/fixBufferEncry.hpp"

int main()
{
    binpi v = { 1,2.678 };
    size_t readbyte = 0;
    auto buffer = encryption::BackFixEncry<encryption::typeEncry<binpi>, 1, 2, 3>::Encode<std::vector<unsigned char>>(v);
    auto value = encryption::BackFixEncry<encryption::typeEncry<binpi>, 1, 2, 3>::Decode(buffer.begin(), buffer.end(),&readbyte);
    std::cout << std::to_string(value) << std::endl;
    return 0;
}
	*/
	template<ConceptEncryption Encry, unsigned char...fixElement>
	struct BackFixEncry
	{
		static constexpr std::array<unsigned char, sizeof...(fixElement)> fixElementArray = { fixElement... };
		__SIGNED_ENCRYPTION
		using encodeType = Encry::encodeType;
		template<typename EncodeContainerType>
		static EncodeContainerType Encode(encodeType& data)
		{
			auto EncryData = Encry::template Encode<EncodeContainerType>(data);
			EncryData.insert(std::end(EncryData), std::begin(fixElementArray), std::end(fixElementArray));
			return std::move(EncryData);
		}

		template<typename iterator>
		static encodeType Decode(iterator begin, iterator end, size_t* readbyte)
		{
			size_t dataSize = end - begin;
			__error_throw_out_UserErrorCode(dataSize >= fixElementArray.size(), DataProtocolStatue::LACK_DATA, "data error")

			size_t n = 0;
			auto ret = Encry::template Decode(begin, end, &n);
			begin = begin + n;
			for (auto& item : fixElementArray) {
				__error_throw_out_UserErrorCode(item == static_cast<unsigned char>(*begin), DataProtocolStatue::ERROR_DATA, "data error")
				begin++;
			}
			if (readbyte)
				*readbyte = n + fixElementArray.size();
			return std::move(ret);
		}
	};
}