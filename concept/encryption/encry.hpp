#pragma once
#include <boost/hana/assert.hpp>
#include <boost/hana/equal.hpp>
#include <boost/hana/ext/boost/fusion/vector.hpp>
#include <boost/hana/integral_constant.hpp>
#include <boost/hana/transform.hpp>

#include <boost/hana/tuple.hpp>
#include <boost/hana/type.hpp>
#include <boost/hana/for_each.hpp>
#include <boost/hana/range.hpp>
#include <tuple>

#include "concept/error/errorbase.hpp"
#include "concept/encryption/encryHead.hpp"
NAMESPACE_BIN_ENCRY
{

	template<ConceptEncryption ...Encrys>
	struct linkEncryptionEncry
	{
		using encodeType = std::tuple<typename Encrys::encodeType...>;
		__SIGNED_ENCRYPTION
		/*
		* 生成一个tuple的表，大概就是
		* <<Args[0],0>,<Args[0],1>...>这样的方便后面std::get<N>(t)来获取参数
		*/
		static constexpr auto TypeMap = boost::hana::unpack(boost::hana::tuple_t<Encrys...>, [&]<typename... params>(params... args) {
			return boost::hana::unpack(boost::hana::make_range(boost::hana::int_c<0>, boost::hana::int_c<sizeof ...(Encrys)>), [&]<typename... IntParams>(IntParams... args) {
				return  boost::hana::make_tuple(boost::hana::make_pair(boost::hana::type_c<Encrys>, boost::hana::int_c<IntParams::value>)...);
			});

		});

		template<typename EncodeContainerType = std::vector<unsigned char>>
		static EncodeContainerType Encode(encodeType& data)
		{
			EncodeContainerType ret;
			boost::hana::for_each(TypeMap, [&]<typename params>(params args) {
				boost::hana::unpack(args, [&]<typename EcodeType, typename TypeIndex>(EcodeType a, TypeIndex b) {
					//实现比如<Args[0],0>,这里就是Args[0]::type::Encode(std::get<0>(data));
					auto midData = EcodeType::type::template Encode<EncodeContainerType>(std::get<TypeIndex::value>(data));
					ret.insert(ret.end(), std::begin(midData), std::end(midData));
					});
				});
			return std::move(ret);
		}

		template<typename iterator>
		static encodeType Decode(iterator begin, iterator end, size_t* readbyte)
		{
			encodeType ret;
			size_t Readbyte = 0;
			auto DataSize = end - begin;
			iterator newbegin = begin;
			boost::hana::for_each(TypeMap, [&]<typename params>(params args) {
				boost::hana::unpack(args, [&]<typename EcodeType, typename TypeIndex>(EcodeType a, TypeIndex b) {
					//实现比如<Args[0],0>,这里就是Args[0]::type::Encode(std::get<0>(data));  
					size_t oneTimeReadbyte = 0;
					std::get<TypeIndex::value>(ret) = EcodeType::type::template Decode<iterator>(newbegin, end, &oneTimeReadbyte);
					Readbyte += oneTimeReadbyte;
					newbegin = newbegin + oneTimeReadbyte;
					__error_throw_out_UserErrorCode(newbegin - begin <= DataSize, DataProtocolStatue::LACK_DATA, "buffer too small")
					});
			});
			if (readbyte) {
				*readbyte = Readbyte;
			}
			return std::move(ret);
		}

	};
}