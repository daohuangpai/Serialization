#pragma once
#include <system_error>
#include <format>
#include<sstream>
#include <concept/model/singleton.hpp>
#include "concept/error/commonException.hpp"
namespace youla
{
	struct debug_info
	{
		std::string_view file;
		std::string_view call_function_name;
		long line;
    };
}
namespace std
{
	_NODISCARD inline string to_string(const youla::debug_info&& _Val) { // convert debug_info to string		
        return std::format("file:{}\r\nfunction:{}\r\nline:{}\r\n", _Val.file, _Val.call_function_name, _Val.line);
	}
}

typedef std::system_error concept_error;
#define __USER_ERROR 0xff0


 

class empty_error_category : public std::error_category
{
public:
	using base = std::error_category;
	using base::base;
	virtual const char* name() const noexcept override
	{
		return  typeid(*this).name();
	}
	virtual std::string message(int _Errcode) const noexcept override
	{
		return "";
	}

	//static empty_error_category static_category;
};

#define __UNKNOW_ERROR  concept_error(0xf1,model::singleton<empty_error_category>::instance(),"unknow error!")

#define __error_debug_info() youla::debug_info({__FILE__,__FUNCTION__,__LINE__})
//#define __error_throw_out(assert_conditional,except, ...) if(!(assert_conditional)){__VA_ARGS__;throw except;}
#define __error_debug_string(str) (std::to_string(__error_debug_info())+str)

#define __error_throw(assert_conditional,errorType,...)\
         if(!(assert_conditional)){\
          __VA_ARGS__\
		 throw errorType;\
        }
 
#define __error_throw_out_msg(assert_conditional,code,category,str, ...) \
       if(!(assert_conditional)){\
         __VA_ARGS__\
         auto msg =  __error_debug_string(str);\
		throw concept_error(std::error_code(code,category), msg);\
      }

#define __error_throw_out_just_msg(assert_conditional,str,...) __error_throw_out_msg(assert_conditional,0,helper::singleton<empty_error_category>::instance(),str, __VA_ARGS__)

#define __error_throw_out(assert_conditional,code,category, ...) \
       if(!(assert_conditional)){\
        __VA_ARGS__\
		throw concept_error(std::error_code(code,category), std::to_string(__error_debug_info()));\
      }

#define __error_throw_out_ErrorCode(assert_conditional,code,str, ...) \
       if(!(assert_conditional)){\
         __VA_ARGS__\
         {\
          auto err = std::error_code(code);\
          auto msg =  __error_debug_string(str);\
         throw concept_error(err, msg); \
          }\
       }
#define __error_throw_out_Class_Code(assert_conditional,code,str, ...) \
       if(!(assert_conditional)){\
         __VA_ARGS__\
         {\
          auto msg =  __error_debug_string(str);\
         throw concept_error(code, msg); \
          }\
       }

#define __waring_throw_out(assert_conditional,text)\
std::cout << __FUNCTION__ << "waring:" << text << std::endl;
/*
* enum class tre
{
    HEKL =1,
    asiujdfh,
    asojkhdlkoa,
};
* int main()
{
    try {
        try {
            __error_throw_out_UserErrorCode(false, tre::asiujdfh, "hello error") 
         }
         catch (ValueException<tre>& e)
        {
           if(e.TestRethrowValue(tre::asiujdfh))
               std::printf("ValueException : %s\n", e.what());
         }
    }
    catch (std::exception& e)
    {
        std::printf("exception : %s\n", e.what());
    }
    return 0;
}
���
ValueException : ValueException:file:������.cpp
function:main
line:46
hello error
 code:asiujdfh

 �ڵ�һ��catch�ͽ�ȡ��

 ���if(e.TestRethrowValue(tre::asiujdfh))��ƥ��������std::rethrow_exception(std::current_exception());
 ��ô������������ϼ����ݵ���
     catch (std::exception& e)
    {
        std::printf("exception : %s\n", e.what());
    }

 ����

 int main()
{
    try {
        try {
            __error_throw_out_UserErrorCode(false, tre::asiujdfh, "hello error")
         }
         catch (ValueException<tre>& e)
        {
           if(e.TestRethrowValue(tre::HEKL))
               std::printf("ValueException : %s\n", e.what());
         }
    }
    catch (std::exception& e)
    {
        std::printf("exception : %s\n", e.what());
    }
    return 0;
}
���
exception : ValueException:file:������.cpp
function:main
line:46
hello error
 code:asiujdfh

 ʹ��������԰������Ϊһ�������봦��
*/
#define __RETHROW__ std::rethrow_exception(std::current_exception());
 
#define __error_throw_out_UserErrorCode(assert_conditional,code,str, ...) \
       if(!(assert_conditional)){\
         __VA_ARGS__\
         {\
          auto msg =  __error_debug_string(str);\
         throw ValueException<decltype(code)>(msg,code); \
          }\
       }
 