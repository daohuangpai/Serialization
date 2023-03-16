#pragma once

namespace base
{
	class noncopyable {
	private:
		const noncopyable& operator=(const noncopyable&) = delete;
		noncopyable(const noncopyable&) = delete;
	protected:		
		noncopyable() = default;
		~noncopyable() = default;	
		
	};
}
 