#pragma once
#include "concept/reflector/reflectorSerialize.hpp"
#include "concept/encryption/lengthEncry.hpp"
NAMESPACE_BIN_ENCRY
{
	/*
	* 这个解码时没办法获得数据大小，只能使用lengthEncry包装，lengthEncry在处理ConceptEncryptionKindType时使用的是length的部分包而不是全部
	没办法获得解码的数据大小也就没办法单独成为coder，所以只能借助lengthEncry这种既定数据大小的方法ReflectorDataJsonEncry
	*/
	template<typename T>
	requires ReflectorSerializeStruct<T>
	struct ReflectorDataJsonGenerater
	{
		using encodeType = T;
		template<typename EncodeContainerType>
		static EncodeContainerType Encode(encodeType& data)
		{
			EncodeContainerType ret;
			std::stringstream ss;
			{
				cereal::JSONOutputArchive archive(ss);
				archive(data);
			}
			std::string dataStr = ss.str();
			ret.insert(std::end(ret), std::begin(dataStr), std::end(dataStr));

			return std::move(ret);
		}

		template<typename iterator>
		static encodeType Decode(iterator begin, iterator end,size_t* readbyte)
		{
			encodeType ret;
			std::stringstream ss;
			std::string sh;
			sh.assign(begin, end);
			ss << sh;
			cereal::JSONInputArchive ar(ss);
			ar(ret);
			if(readbyte)
			   *readbyte = end - begin;
			return std::move(ret);
		}
	};
	template<typename T, typename LengthType = std::uint64_t, endian E = NATIVE>
	using ReflectorDataJsonEncry = lengthEncry<ReflectorDataJsonGenerater<T>, LengthType, E>;

}