#pragma once
#include <iostream>
#include <boost/endian/conversion.hpp> 
#include "concept/error/errorbase.hpp"
#include "concept/encryption/typeEncry.hpp"
NAMESPACE_BIN_ENCRY
{
	/*
	* length data
	* 这样的结构
	* 前面是长度，后面是数据
	*/
	/*
	* 对长度的编码方式，小端，不处理，大端，用于网络传输
	*/
	enum endian {
		BIG_ENDIAN = 1,
		NATIVE,
		LITTLE_ENDIAN
	};

	enum class lengthEncryErrorCode{
		LENGTH_TYPE_TO_SMALL,//长度类型不够存数据大小
		DATA_LENGTH_NOT_FIT
	};

	
	template<typename T, endian E = encryption::NATIVE,std::enable_if_t<std::is_integral_v<T>, int> = 0>
	struct NetNumberEncry {
		using encodeType = T;
		constexpr static auto BufferByteNeed = sizeof(encodeType);
		__SIGNED_ENCRYPTION

		 
		template<typename EncodeContainerType>
		static EncodeContainerType Encode(encodeType& data)
		{
			encodeType value;
			if constexpr (E == encryption::LITTLE_ENDIAN) {
				value = boost::endian::native_to_little(data);
			}
			else if constexpr (E == encryption::BIG_ENDIAN) {
				value = boost::endian::native_to_big(data);
			}
			else if constexpr (E == encryption::NATIVE) {
				value = data;
			}
			else {
				//static_assert(false, "unsupport endian type");会直接报错，所以用这种方式
				static_assert(E != encryption::LITTLE_ENDIAN, "unsupport endian type");
			}
			return std::move(typeEncry<encodeType>::template Encode<EncodeContainerType>(value));
		}

		template<typename iterator>
		static encodeType Decode(iterator begin, iterator end, size_t* readbyte)
		{
			encodeType data = typeEncry<encodeType>::template Decode(begin, end, readbyte);
			encodeType value ;
			if constexpr (E == encryption::LITTLE_ENDIAN) {
				value = boost::endian::little_to_native(data);
			}
			else if constexpr (E == encryption::BIG_ENDIAN) {
				value =  boost::endian::big_to_native(data);
			}
			else if constexpr (E == encryption::NATIVE) {
				value = data;
			}
			else {
				//static_assert(false, "unsupport endian type");会直接报错，所以用这种方式
				static_assert(E != encryption::LITTLE_ENDIAN, "unsupport endian type");
			}
			return std::move(value);
		}
	};




	template<ConceptEncryptionKindType Encry, typename LengthType = std::uint64_t, endian E = encryption::NATIVE>
	struct lengthEncry
	{
		using encodeType = Encry::encodeType;
		using lengthCoderType = NetNumberEncry<LengthType, E>;
		__SIGNED_ENCRYPTION
		/*
		* 按下面排序
		* length data
		* 其中length是data的长度
		*/
		template<typename EncodeContainerType>
		static EncodeContainerType Encode(encodeType& data)
		{	
			auto EncryData = Encry::template Encode<EncodeContainerType>(data);
			auto dataSize = std::size(EncryData);
			LengthType length = dataSize;
			//如果dataSize不能用LengthType这个类型装入
			__error_throw_out_UserErrorCode(dataSize == length, lengthEncryErrorCode::LENGTH_TYPE_TO_SMALL, "length type too small")
			EncodeContainerType ret = lengthCoderType::template Encode<EncodeContainerType>(length);
			ret.insert(std::end(ret),std::begin(EncryData),std::end(EncryData));
			return std::move(ret);
		}


		template<typename iterator>
		static encodeType Decode(iterator begin, iterator end, size_t* readbyte)
		{
			iterator newBegin = begin;
			size_t n = 0;
			size_t read = 0;
			auto len = lengthCoderType::template Decode(newBegin, end,&n);
			newBegin = newBegin + n;
			read += n;
			encodeType ret;
			//是为了支持没有办法知道解码数据多少的伪解码器
			if constexpr (ConceptEncryption<Encry>) {
				ret = Encry::template Decode(newBegin, end, &n);
			}
			else {
				iterator newEnd = newBegin + len;
				ret = Encry::template Decode(newBegin, newEnd, &n);
			}
			//ret = Encry::template Decode(newBegin, end, &n);
			//这里的len是指定的后面的数据个数，如果解码后实际数据n和这个len不相等，那么这个结构就是错的了
			__error_throw_out_UserErrorCode(len == n, lengthEncryErrorCode::DATA_LENGTH_NOT_FIT, std::format("length ERROR,need:{},fact:{}", len,n))
			read += n;
			if(readbyte)
			  *readbyte = read;
			return std::move(ret);
		}
	};

}