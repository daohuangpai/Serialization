#pragma once

typedef unsigned char Byte;
typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned long uint32;
typedef unsigned long  long uint64;

typedef char int8;
typedef short int16;
typedef long int32;
typedef long long int64;

typedef unsigned int uint;

#define __STD_FORWARD(p) std::forward<decltype(p)>(p)

#define __struct_member_offset(TYPE, MEMBER) ((size_t) &((TYPE *)0)->MEMBER)  

#define OUT
#define IN 

#define NOEXCEPT  noexcept
#define INLINE inline
#define NODISCARD  [[nodiscard]]
#define FALLTHROUGH  [[fallthrough]]
#define DEPRECATED(warring_string) [[deprecated (warring_string)]]
#define MAYBEUNUSED [[maybe_unused]]
#define NORETURN [[noreturn]] 
#define CONSTEXPR constexpr


#define __enum_index(e) static_cast<unsigned int>(e)