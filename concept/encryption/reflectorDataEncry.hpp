#pragma once
#include "concept/reflector/reflectorSerialize.hpp"
#include "concept/encryption/lengthEncry.hpp"
NAMESPACE_BIN_ENCRY
{
	/*
	* �������ʱû�취������ݴ�С��ֻ��ʹ��lengthEncry��װ��lengthEncry�ڴ���ConceptEncryptionKindTypeʱʹ�õ���length�Ĳ��ְ�������ȫ��
	û�취��ý�������ݴ�СҲ��û�취������Ϊcoder������ֻ�ܽ���lengthEncry���ּȶ����ݴ�С�ķ���ReflectorDataJsonEncry
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