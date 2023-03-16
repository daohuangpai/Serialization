#pragma once
#include <boost/hana/functional/apply.hpp>
#include <boost/hana/plus.hpp>
#include <boost/hana/core/make.hpp>
#include <boost/hana/equal.hpp>
#include <boost/hana/tuple.hpp>
#include "concept\error\errorbase.hpp"
#include <vector>
#define NAMESPACE_BIN_ENCRY namespace encryption

NAMESPACE_BIN_ENCRY
{
	struct encrypAdapter_tag {};
#define __SIGNED_ENCRYPTION using hana_tag = encrypAdapter_tag;
	enum class DataProtocolStatue
	{
		LACK_DATA = 1,
		ERROR_DATA,
		OVER_MANY_DATA
	};
	template<typename T>
	concept  ConceptEncryptionTagType = bool(boost::hana::type_c<typename boost::hana::tag_of<T>::type> == boost::hana::type_c<encrypAdapter_tag>);
 


	template<typename T>
	concept ConceptEncryptionKindType = requires(T encry, typename T::encodeType value, std::vector<unsigned char> code, size_t* readbyte)
	{ 
		T::template Encode<std::vector<unsigned char>>(value);
		T::template Decode(code.begin(),code.end(), readbyte);
	};


	template<typename T>
	concept ConceptEncryption = ConceptEncryptionTagType<T> && ConceptEncryptionKindType<T>;
}
