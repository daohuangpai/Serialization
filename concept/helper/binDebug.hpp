#pragma once

#include <openssl/bio.h>
#include <concept/traits/utilities.hpp>
namespace helper 
{
	template<typename... T>
	inline static void dump(T... value, FILE* f = stdout);
	/*
	* ����ģ�����std::string\std::vector<char>�ȶ���������������ļ����߿���̨չʾ�Ĺ���
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
