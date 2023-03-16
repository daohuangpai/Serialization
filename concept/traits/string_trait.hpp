#pragma once
#include <string>
namespace traits
{
	//�õ��ַ����ṹ��ָ��
	template<typename StrType>
	const char* string_ptr(StrType& str)
	{
		if constexpr (requires(StrType str)
		{
			str.data();
				requires std::is_same_v<std::remove_cv_t<decltype(str.data())>, char*>;
		}
		) {
			return str.data();
		}
		//remove_cv_t�޷��Ƴ�const char*��const
		else if constexpr (std::is_same_v<std::remove_cv_t<StrType>, const char*>
			|| std::is_same_v<std::remove_cv_t<StrType>,char*>) {
			return str;
		}
		//else {
		//	static_assert(false,"error str");
		//}
	}

	//template<typename StrType,
	//	std::enable_if_t<std::is_same_v<std::remove_cv_t<StrType>,char*>,int> = 0>
	//inline const char* string_ptr<StrType>(StrType str)
	//{
	//	return str;
	//}

 
}