#pragma once
#include <vector>
#include "concept/error/errorbase.hpp"

/*
* 
* 类型和本机中二进制的转化
* int转std::vector
        auto a= helper::to_byte<int>::get_byte(890);  a为std::vector<sizeof(int)>
		std::vector转int  b为int
        auto b = helper::to_byte<int>::get_value(a);

		get_value中如果参数的大小和T的大小不一致将会抛出error_type::buffer_error错误

同时to_byte也支持用数组等其他容器
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