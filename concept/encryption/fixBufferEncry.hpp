#pragma once

#include "concept/encryption/encryHead.hpp"
NAMESPACE_BIN_ENCRY
{
	/*
	* 在编码器Encry的前面加上一串固定的字符串
	* 比如下面就是在前面加入1、2、3，本质上只是一层代理
	* 因为
	* using encodeType = Encry::encodeType;
	* 用的编解码类型也是在其子中的
	* 
	* 
	* 要注意这里的fixElement是unsigned char，如果使用超出这个结构体的整数会出现不可预知的问题
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
	* 和上面功能类似，只是把固定的数据加载尾部
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