#pragma once
#include "concept/encryption/encryHead.hpp"
#include <asio.hpp>
NAMESPACE_BIN_ENCRY
{
	//testipEntry.cpp
	template<typename asioIp>
	struct asio_ip_Encry
	{
	   using encodeType = asioIp;
	  

	   /*using bufferType = decltype(std::declval<encodeType>().to_bytes());*/
	   using bufferType = encodeType::bytes_type;
	   constexpr static auto bufferByteSize = sizeof(bufferType);
	   template<typename EncodeContainerType>
	   static EncodeContainerType Encode(encodeType& data)
	   {
		   EncodeContainerType ret;
		   auto byte = data.to_bytes();
		   ret.resize(bufferByteSize);
		   std::copy(std::begin(byte), std::end(byte), std::begin(ret));
		   return std::move(ret);
	   }
	   template<typename iterator>
	   static encodeType Decode(iterator begin, iterator end, size_t* readbyte)
	   {
		  
		   __error_throw_out_UserErrorCode(end - begin >= bufferByteSize, DataProtocolStatue::LACK_DATA, "buffer too small")
		   *readbyte = bufferByteSize;
		   bufferType buffer;
		   std::copy(begin, end, std::begin(buffer));
		   encodeType ret(buffer);
		   return std::move(ret);
	   }
	};

	using Ip4_Encry = asio_ip_Encry<asio::ip::address_v4>;
	using Ip6_Encry = asio_ip_Encry<asio::ip::address_v6>;
}