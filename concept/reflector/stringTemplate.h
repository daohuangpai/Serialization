#pragma once
#include <boost/preprocessor/variadic/elem.hpp>
#include <boost/preprocessor/seq/for_each_i.hpp>
#include <boost/preprocessor/control/if.hpp>
#include <boost/preprocessor/variadic/to_seq.hpp>
#include <boost/preprocessor/seq/seq.hpp>
#include <boost/preprocessor/arithmetic/inc.hpp>
#include <boost/preprocessor/comparison/not_equal.hpp>
#include <boost/preprocessor/repetition/for.hpp>
#include <boost/preprocessor/cat.hpp>
#include <boost/preprocessor/stringize.hpp>
//循环最高PP 256所以这个值小于这个
#define __MAX_TEMPLATE_CHAR 100
#define __MIN(a,b) (a)<(b)?(a):(b)

#define __STRING_TO_TEMPLATE_PRED(r, state) \
   BOOST_PP_NOT_EQUAL( \
      BOOST_PP_TUPLE_ELEM(3, 0, state), \
      BOOST_PP_TUPLE_ELEM(3, 1, state) \
   ) \


#define __STRING_TO_TEMPLATE_OP(r, state) \
   ( \
      BOOST_PP_INC(BOOST_PP_TUPLE_ELEM(3, 0, state)), \
      BOOST_PP_TUPLE_ELEM(3, 1, state), \
      BOOST_PP_TUPLE_ELEM(3, 2, state) \
   ) \

#define __STRING_TO_TEMPLATE_getChr(name, ii) ((__MIN(ii,__MAX_TEMPLATE_CHAR))<sizeof(name)/sizeof(*name)?name[ii]:0)

#define __STRING_TO_TEMPLATE_MACRO(r, state)  __STRING_TO_TEMPLATE_getChr(BOOST_PP_TUPLE_ELEM(3, 2, state),BOOST_PP_TUPLE_ELEM(3, 0, state)), 


#define __STRING_TO_TEMPLATE_CHARS(s) \
        BOOST_PP_FOR((0, __MAX_TEMPLATE_CHAR, s),__STRING_TO_TEMPLATE_PRED, __STRING_TO_TEMPLATE_OP, __STRING_TO_TEMPLATE_MACRO)\
        __STRING_TO_TEMPLATE_getChr(s, __MAX_TEMPLATE_CHAR)


/*
* example:
constexpr static char yh1[] = { __STRING_TO_TEMPLATE_CHARS("hello") };
template<char...args>
struct teststringT;
template<>
struct teststringT<__STRING_TO_TEMPLATE_CHARS("hello")>
{
    using type = int;
};
teststringT<__STRING_TO_TEMPLATE_CHARS("hello")>::type

template<char...args>
struct teststringT
{
    constexpr static char textC[] = { args...};
    constexpr static std::string_view text = textC;
};
constexpr static auto aaa = teststringT<__STRING_TO_TEMPLATE_CHARS("hello")>::text;//就是"hello"，不用担心最后一个是不是0因为__STRING_TO_TEMPLATE_CHARS定义了最后一个是0
*/

/*
#define PRED(r, state) \
   BOOST_PP_NOT_EQUAL( \
      BOOST_PP_TUPLE_ELEM(2, 0, state), \
      BOOST_PP_INC(BOOST_PP_TUPLE_ELEM(2, 1, state)) \
   ) \
  

#define OP(r, state) \
   ( \
      BOOST_PP_INC(BOOST_PP_TUPLE_ELEM(2, 0, state)), \
      BOOST_PP_TUPLE_ELEM(2, 1, state) \
   ) \

#define MACRO(r, state) BOOST_PP_TUPLE_ELEM(2, 0, state)  
//#define MACRO(r, state) r
constexpr static auto fff = BOOST_PP_STRINGIZE(BOOST_PP_FOR((5, 10), PRED, OP, MACRO));

结果是输出 5 6 7 8 9 10
r输出是    2 3 4 5 6 7
https://www.boost.org/doc/libs/1_49_0/libs/preprocessor/doc/index.html
过程
BOOST_PP_FOR(state, pred, op, macro)
state是初始状态，可以是任何宏
1、state 
This macro expands to the sequence:
macro(r, state) macro(r, op(r, state)) ... macro(r, op(r, ... op(r, state) ... ))
第一次是state，第二次就是op(r, state)。。。
2、r输出是代表for中的序号

pred(r,state) 如果是0则for终止，判断参数  pred(r,op(r, state)) pred(r, op(r, state)) ... pred(r, op(r, ... op(r, state) ... ))
op(r, state)  运算作为下一次的pred的参数，判断参数
macro(r, state)  就是具体展开的值的输出，每一次都是输出一个值，如例子中MACRO(r, state) BOOST_PP_TUPLE_ELEM(2, 0, state)  就是指state中的第一个值作为输出
例子中
   BOOST_PP_NOT_EQUAL( \
      BOOST_PP_TUPLE_ELEM(2, 0, state), \
      BOOST_PP_INC(BOOST_PP_TUPLE_ELEM(2, 1, state)) \
   ) \
   虽然对BOOST_PP_INC(BOOST_PP_TUPLE_ELEM(2, 1, state)但是实际上由于
   #define OP(r, state) \
   ( \
      BOOST_PP_INC(BOOST_PP_TUPLE_ELEM(2, 0, state)), \
      BOOST_PP_TUPLE_ELEM(2, 1, state) \
   ) \
   所以传给pred的state第二个值一直是固定的，所以一直是10+1

*/




/*idear from https://stackoverflow.com/questions/1826464/c-style-strings-as-template-arguments
* #define MAX_CONST_CHAR 100

#define MIN(a,b) (a)<(b)?(a):(b)

#define _T(s)\
getChr(s,0),\
getChr(s,1),\
getChr(s,2),\
getChr(s,3),\
getChr(s,4),\
getChr(s,5),\
getChr(s,6),\
getChr(s,7),\
getChr(s,8),\
getChr(s,9),\
getChr(s,10),\
getChr(s,11),\
getChr(s,12),\
getChr(s,13),\
getChr(s,14),\
getChr(s,15),\
getChr(s,16),\
getChr(s,17),\
getChr(s,18),\
getChr(s,19),\
getChr(s,20),\
getChr(s,21),\
getChr(s,22),\
getChr(s,23),\
getChr(s,24),\
getChr(s,25),\
getChr(s,26),\
getChr(s,27),\
getChr(s,28),\
getChr(s,29),\
getChr(s,30),\
getChr(s,31),\
getChr(s,32),\
getChr(s,33),\
getChr(s,34),\
getChr(s,35),\
getChr(s,36),\
getChr(s,37),\
getChr(s,38),\
getChr(s,39),\
getChr(s,40),\
getChr(s,41),\
getChr(s,42),\
getChr(s,43),\
getChr(s,44),\
getChr(s,45),\
getChr(s,46),\
getChr(s,47),\
getChr(s,48),\
getChr(s,49),\
getChr(s,50),\
getChr(s,51),\
getChr(s,52),\
getChr(s,53),\
getChr(s,54),\
getChr(s,55),\
getChr(s,56),\
getChr(s,57),\
getChr(s,58),\
getChr(s,59),\
getChr(s,60),\
getChr(s,61),\
getChr(s,62),\
getChr(s,63),\
getChr(s,64),\
getChr(s,65),\
getChr(s,66),\
getChr(s,67),\
getChr(s,68),\
getChr(s,69),\
getChr(s,70),\
getChr(s,71),\
getChr(s,72),\
getChr(s,72),\
getChr(s,72),\
getChr(s,73),\
getChr(s,74),\
getChr(s,75),\
getChr(s,76),\
getChr(s,77),\
getChr(s,78),\
getChr(s,79),\
getChr(s,80),\
getChr(s,81),\
getChr(s,82),\
getChr(s,83),\
getChr(s,84),\
getChr(s,85),\
getChr(s,86),\
getChr(s,87),\
getChr(s,88),\
getChr(s,89),\
getChr(s,90),\
getChr(s,91),\
getChr(s,92),\
getChr(s,93),\
getChr(s,94),\
getChr(s,95),\
getChr(s,96),\
getChr(s,97),\
getChr(s,98),\
getChr(s,99),\
getChr(s,100)

//如果是在定义的区域也就是小于100字节就用字符串原文，否则用0
#define getChr(name, ii) ((MIN(ii,MAX_CONST_CHAR))<sizeof(name)/sizeof(*name)?name[ii]:0)
template <char... Chars_>
class E {

public:
    std::string* str;

    E() {
        std::vector<char> vec = { Chars_... };
        str = new std::string(vec.begin(), vec.end());
    }

    ~E()
    {
        delete str;
    }
};
E<_T("Any template can pass const strings literals")> e;
*/