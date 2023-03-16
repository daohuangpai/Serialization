#pragma once
#include "concept/encryption/lengthEncry.hpp"

NAMESPACE_BIN_ENCRY
{
	template<typename T>
	struct bufferEncryHelper
	{
	  using encodeType = T;

	  template<typename EncodeContainerType>
	  static EncodeContainerType Encode(encodeType& data)
	  {
		  EncodeContainerType ret(std::begin(data), std::end(data));
		  return std::move(ret);
	  }

	  template<typename iterator>
	  static encodeType Decode(iterator begin, iterator end, size_t* readbyte)
	  {
		  encodeType ret(begin, end);
		  *readbyte = end - begin;
		  return std::move(ret);
	  }
	};
    
    template<typename T,typename LengthType = std::uint64_t, endian E = NATIVE>
	requires (requires(T v) { std::begin(v); std::end(v); })
	using bufferEncry = lengthEncry<bufferEncryHelper<T>, LengthType, E>;

}