#pragma once
#include <vector>
#include "concept/error/errorbase.hpp"

/*
* 
* ���ͺͱ����ж����Ƶ�ת��
* intתstd::vector
        auto a= helper::to_byte<int>::get_byte(890);  aΪstd::vector<sizeof(int)>
		std::vectorתint  bΪint
        auto b = helper::to_byte<int>::get_value(a);

		get_value����������Ĵ�С��T�Ĵ�С��һ�½����׳�error_type::buffer_error����

ͬʱto_byteҲ֧�����������������
int main()
{
	try {
	   auto a= helper::to_byte<int>::get_byte(890);
	   char byte[4] = { 1,2,3,4 };
	   std::copy(std::begin(a),std::end(a),byte);
	   auto b = helper::to_byte<int>::get_value(byte);
	}
	catch(std::exception& e)
	{
		std::printf("exception : %s\n", e.what());
	}
}
*/
namespace helper
{
	enum class to_byteErrorCode {
		buffer_error
	};
	template<typename T,typename byteType = unsigned char>
	struct to_byte {
#pragma pack(1)
		union to_byte_helper
		{
			T v;
			byteType byte[sizeof(T)];
		};
#pragma pack()
		template<typename container = std::vector<byteType>>
		static container get_byte(T v)
		{
			to_byte_helper h;
			h.v = v;
			return std::move(container(std::begin(h.byte), std::end(h.byte)));
		}



		template<typename iterator>
		static T get_value(iterator begin, iterator end)
		{
			to_byte_helper h;
			__error_throw_out_UserErrorCode(end - begin == sizeof(T), to_byteErrorCode::buffer_error, "to_byte error")
				std::copy(begin, end, h.byte);
			return h.v;
		}

		template<typename container = std::vector<byteType>>
		static T get_value(container& buffer)
		{
			return get_value(std::begin(buffer), std::begin(buffer));
		}
	};
}