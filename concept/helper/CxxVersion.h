#pragma once

#if defined(__clang__)|| defined(__GNUC__)
#define CPP_VER __cplusplus
#elif defined(_MSC_VER)
#define CPP_VER _MSVC_LANG
#endif

#if CPP_VER >= 199711L
#define _CPP_03
#endif
#if CPP_VER >= 201103L
#define _CPP_11
#endif
#if CPP_VER >= 201402L
#define _CPP_14
#endif
#if CPP_VER >= 201703L
#define _CPP_17
#endif
#if CPP_VER >= 202004L
#define _CPP_20
#endif
