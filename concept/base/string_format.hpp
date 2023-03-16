#pragma once

#include<string>
#include "define.h"
namespace std_string
{
    //registered type with return value_type
	template <class type>
	struct conver_format 
	{
		using value_type = const type&&; 
	};
	template <>
	struct conver_format<std::string_view>
	{
		using value_type = const char*;
	};
	template <>
	struct conver_format<std::string>
	{
		using value_type = const char*;
	};
    
	template<typename InputType>
	INLINE typename conver_format<InputType>::value_type to_format_type(const InputType&& input)
	{
		return __STD_FORWARD(input);
	}	
	template<>
	INLINE typename conver_format<std::string_view>::value_type to_format_type<std::string_view>(const std::string_view&& input)
	{
		return input.data();
	}
	template<>
	INLINE typename conver_format<std::string>::value_type to_format_type<std::string>(const std::string&& input)
	{
		return input.data();
	}

	template<class char_type,typename... Args>
	std::enable_if_t<std::is_same<std::decay_t<char_type>, char>::value, std::string> Format(char_type* str_format, Args...args)
	{
		const auto _Len = static_cast<size_t>(_CSTD _scprintf(str_format, to_format_type(__STD_FORWARD(args))...));
		std::string _Str(_Len, '\0');
		_CSTD sprintf_s(&_Str[0], _Len + 1, str_format, to_format_type(__STD_FORWARD(args))...);
		return std::move(_Str);
	}
 
	template<class char_type, typename... Args>
	std::enable_if_t<std::is_same<std::decay_t<char_type>, wchar_t>::value, std::wstring> Format(char_type* str_format, Args...args)
	{
		const auto _Len = static_cast<size_t>(_CSTD _scwprintf(str_format, to_format_type(__STD_FORWARD(args))...));
		std::wstring _Str(_Len, '\0');
		_CSTD swprintf_s(&_Str[0], _Len + 1, str_format, to_format_type(__STD_FORWARD(args))...);
		return std::move(_Str);
	}





 
}