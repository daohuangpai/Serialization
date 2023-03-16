#pragma once
#include "concept/encryption/typeEncry.hpp"
#include <asio.hpp>
NAMESPACE_BIN_ENCRY
{
	template<typename LengthType>
	struct netLengthEncry;

	/*
	htonl()–“Host to Network Long”
	ntohl()–“Network to Host Long”
	htons()–“Host to Network Short”
	ntohs()–“Network to Host Short”
	*/
	using netLengthShortType = decltype(::htons(0));
	using netLengthLengthType = decltype(::htonl(0));

	template<typename LengthType>
	struct netLengthEncry
	{
		using encodeType = LengthType;
		template<typename EncodeContainerType>
		static EncodeContainerType Encode(encodeType& data)
		{
			encodeType value;
			if constexpr (sizeof(encodeType) == sizeof(char)) {
				value = data;
			}
			else if constexpr (std::is_same_v<encodeType, netLengthShortType>) {
				value = ::htons(data);
			}
			else if constexpr (std::is_same_v<encodeType, netLengthLengthType>) {
				value = ::htonl(data);
			}
			else {
				//static_assert(false, "unsupport endian type");会直接报错，所以用这种方式
				static_assert(sizeof(encodeType) == sizeof(char), "unsupport endian type");
			}
			return std::move(typeEncry<encodeType>::template Encode<EncodeContainerType>(value));
		}

		template<typename iterator>
		static encodeType Decode(iterator begin, iterator end, size_t* readbyte)
		{
			encodeType data = typeEncry<encodeType>::template Decode(begin, end, readbyte);
			encodeType value;
			if constexpr (sizeof(encodeType) == sizeof(char)) {
				value = data;
			}
			else if constexpr (std::is_same_v<encodeType, netLengthShortType>) {
				value = ::ntohs(data);
			}
			else if constexpr (std::is_same_v<encodeType, netLengthLengthType>) {
				value = ::ntohl(data);
			}
			else {
				//static_assert(false, "unsupport endian type");会直接报错，所以用这种方式
				static_assert(sizeof(encodeType) == sizeof(char), "unsupport endian type");
			}
			return std::move(value);
		}
	};

}