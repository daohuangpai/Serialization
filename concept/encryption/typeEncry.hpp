#pragma once
#include "concept/helper/to_byte.hpp"
#include "concept/encryption/encryHead.hpp"


/*
* 
* ���ڶ�����һ���࣬���࣬����ʲô���ݶ������е�����ж����ƻ�
* ʹ��ʵ������
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
���  1,2.678000,8,8
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
		//����ṹ��Ĵ�С��ԭʼ�����ݴ�С����һ������ô���溯����ȫ���߼������ܳ�������������û������ȫ�Ǵ��
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