#pragma once
#include <system_error>
#include <magic_enum.hpp>
#include <format>
namespace std
{
	template<typename EnumType,
		std::enable_if_t<std::is_enum_v<EnumType>, int> = 0>
	std::string to_string(EnumType value)
	{
		return magic_enum::enum_name(static_cast<EnumType>(value)).data();
	}
}

template<typename InfoType>
class ValueException :public std::exception {
public:
	 /*using std::exception::std::exception;*/
	ValueException(char const* const _Message,InfoType value)
		 :std::exception(std::format("ValueException:{} \r\n code:{}", _Message,std::to_string(value)).data()),
		  m_value(value)
	 {
	 }
	ValueException(std::string& _Message, InfoType value)
		:ValueException(_Message.data(), value)
	{
	}
	 InfoType get_value() {
		 return m_value;
	 }
     
	 //检测数据，如果不相等则重新向上级抛出错误
	 bool TestRethrowValue(InfoType value)
	 {
		 if (m_value != value) {
			 std::rethrow_exception(std::current_exception());
		 }
		 return true;
	 }
private:
	InfoType m_value;
};
