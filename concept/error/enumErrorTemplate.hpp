/*
* 枚举自动生成错误码类型
* 
*/
#include "concept/ThirdParty/magic_enum.hpp"
#include "concept/model/singleton.hpp"
#include <system_error>


/*
定义枚举
enum class tre
{
	HEKL =1,
	asiujdfh,
	asojkhdlkoa,
};
int main()
{
	try {
		__error_throw_out_ErrorCode(false, std::EnumErrorCode<tre>(tre::asojkhdlkoa),"qe")
	}
	catch (std::exception& e)
	{
		std::printf("%s\n", e.what());
	}
	return 0;
}
输出：
file:...
function:main
line:39
qe: enum tre errorCode:asojkhdlkoa

就是qe: enum tre errorCode:asojkhdlkoa
把枚举转成字符
*/
_STD_BEGIN
template <typename EnumType, 
	std::enable_if_t<std::is_enum_v<EnumType>, int> = 0>
class EnumErrorCode {
public:
	EnumErrorCode() = delete;
	EnumErrorCode(EnumType code) :m_code(code) {}
	int operator()() { return static_cast<int>(m_code); }
private:
	EnumType m_code;
};

template <typename EnumType,
	std::enable_if_t<std::is_enum_v<EnumType>, int> = 0>
class EnumErrorCodeCategory : public std::error_category {
public:
	 
	EnumErrorCodeCategory() {}
	const char* name() const noexcept override {
		return typeid(EnumType).name();
	}
	std::string message(int err) const override {
		return std::move(std::format("{} errorCode:{}", name(),magic_enum::enum_name(static_cast<EnumType>(err)).data()));
	}
};



template <typename EnumType>
struct is_error_code_enum<EnumErrorCode<EnumType>> : public true_type {};


template <typename EnumType,
	std::enable_if_t<std::is_enum_v<EnumType>, int> = 0>
	_NODISCARD error_code make_error_code(EnumErrorCode<EnumType> e) noexcept {
	return error_code(static_cast<int>(e()), *model::singleton<EnumErrorCodeCategory<EnumType>>::getInstance());
}
_STD_END