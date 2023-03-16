#pragma once

#include <openssl/bio.h>
#include <concept/traits/utilities.hpp>
namespace helper 
{
	template<typename... T>
	inline static void dump(T... value, FILE* f = stdout);
	/*
	* 用于模板比如std::string\std::vector<char>等二进制数据输出到文件或者控制台展示的工具
	*/
	template<typename T>
	inline static void dump(T& value,FILE* f = stdout)
	{
		BIO_dump_fp(f, value.data(), value.size());
	}

	template<typename binary_pointer_type,typename length_type>
	//requires (traits::is_binary_pointer_v<binary_pointer_type>&& traits::is_length_type_v<length_type>)
	inline static void dump(binary_pointer_type pointer, length_type length, FILE* f = stdout)
	{
		BIO_dump_fp(f, pointer, length);
	}
}
