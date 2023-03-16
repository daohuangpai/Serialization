#pragma once
#include "concept/helper/to_byte.hpp"
#include "concept/encryption/encryHead.hpp"


/*
* 
* 用于对任意一个类，简单类，就是什么内容都在类中的类进行二进制化
* 使用实例如下
struct binpi {
	uint32 p;
	float yu;
};

int main()
{
	binpi v = { 1,2.678 };
	auto buffer = encryption::typeEncry<binpi>::Encode<std::vector<unsigned char>>(v);
	std::vector<unsigned char> bufferTotal;
	bufferTotal.insert(std::end(bufferTotal), std::begin(buffer), std::end(buffer));
	bufferTotal.push_back(12);
	bufferTotal.push_back(456);
	size_t readbyte = 0;
	binpi vDecode = encryption::typeEncry<binpi>::Decode(std::begin(bufferTotal), std::end(bufferTotal), &readbyte);
	assert(readbyte == sizeof(binpi));
	printf("%d,%f,%d,%d", vDecode.p, vDecode.yu, sizeof(binpi), readbyte);
	return 0;
}
输出  1,2.678000,8,8
*/


NAMESPACE_BIN_ENCRY{
	template<typename T>
	struct typeEncry
	{
		using encodeType = T;

#pragma pack(1)
		union to_byte_helper
		{
			encodeType v;
			unsigned char byte[sizeof(encodeType)];
		};
#pragma pack()
		//如果结构体的大小和原始的数据大小不能一样，那么后面函数的全部逻辑都不能成立这个函数类就没有意义全是错的
		static_assert(sizeof(to_byte_helper) == sizeof(encodeType), "unsupport typeEncry because target to_byte_helper size is different from encodeType!");
		
		constexpr static auto BufferByteNeed = sizeof(encodeType);
		__SIGNED_ENCRYPTION
	/*	using helperType =typename helper::to_byte<encodeType>;*/
		template<typename EncodeContainerype>
		static EncodeContainerype Encode(encodeType& data)
		{
			to_byte_helper h;
			h.v = data;
			return std::move(EncodeContainerype(std::begin(h.byte), std::end(h.byte)));
		}

		template<typename iterator>
		static encodeType Decode(iterator begin, iterator end,size_t* readbyte)
		{
			__error_throw_out_UserErrorCode(BufferByteNeed <= end - begin, DataProtocolStatue::LACK_DATA, "buffer too small")
			to_byte_helper h;
			iterator newEnd = begin + BufferByteNeed;
			std::copy(begin, newEnd, h.byte);
			if(readbyte)
			  *readbyte = BufferByteNeed;
			return std::move(h.v);
		}

	};

}