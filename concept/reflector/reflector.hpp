#pragma once

#include <boost\type_traits\add_const.hpp>
#include <boost/preprocessor/variadic/elem.hpp>
#include <boost/preprocessor/seq/for_each_i.hpp>
#include <boost/preprocessor/control/if.hpp>
#include <boost/preprocessor/variadic/to_seq.hpp>
#include <boost/preprocessor/seq/seq.hpp>
#include <boost/mpl/for_each.hpp>
#include <boost/mpl/range_c.hpp>
#include <boost/bind.hpp>
#include "concept/traits/traits.hpp"
#include <tuple>        
#include <string>
#include <concept/helper/CxxVersion.h>
#include <iostream>
namespace Reflector
{
    /*
    * 如果有继承的子类也有反射也需要表现，那么就需要将他加入到registeredReflector中并继承
    * bug：默认都是使用public继承，其实使用private也可以做到遍历，所以这个
    * 缺陷：要做到全部都继承这个东西，要在这个类中注册，没注册的就算继承也不行
    * registeredReflectorAndInherit有一定的容错力，即使加入不是具有反射性质的也能正确表达，但这个也算缺点？
    */
    template<class ...Args>
    class registeredReflectorAndInherit;
    template<class Arg0>
    class registeredReflectorAndInherit<Arg0> :public Arg0
    {
    public:
        using reflectorClass = std::tuple<Arg0>;
    };
    template<class Arg0, class ...Args>
    class registeredReflectorAndInherit<Arg0, Args...>:public Arg0,public registeredReflectorAndInherit<Args...>
    {
    public:
        using reflectorClass = std::tuple<Arg0, Args...>;
        virtual ~registeredReflectorAndInherit<Arg0, Args...>() {}
    };
    //定义模板hasreflectorClass
    __defineHasClassMember(reflectorClass)

    template<typename T, class = std::void_t<>>
    struct hasfield_dataClass :public std::false_type
    {
    };
    template<typename T>
    struct hasfield_dataClass<T, std::void_t<typename T::template field_data<0, T>>> :public std::true_type
    {
    };
    template<typename T>
    INLINE constexpr auto hasfield_dataClass_v = hasfield_dataClass<T>::value;

    template<typename T>
    concept ReflectorClass = hasfield_dataClass_v<T>;
    
    template<class M, class T>
    struct make_const
    {
        typedef T type;
    };

    template<class M, class T>
    struct make_const<const M, T>
    {
        typedef typename boost::add_const<T>::type type;
    };

    struct reflector
    {
        //Get field_data at index N
        /*
        * T是使用了REFLECTABLE注册的反射类，N是第几个
        */
        template<int N, class T>
        static typename T::template field_data<N, T> get_field_data(T& x)
        {
            return typename T::template field_data<N, T>(x);
        }

        // Get the number of fields
        /*
        * 得到REFLECTABLE注册的变量个数
        */
        template<class T>
        struct fields
        {
            static const int n = T::fields_n;
        };
    };

    /*
    * 相当于全局函数field_visitor(C& c, Visitor v, I)
    * 这样写方便作为模板参数
    *  C是反射类，Visitor是上面的回调函数，接收参数C::field_data<N, T>就是对应的某一个反射类
    * I就是一个类具有I::value描述一个数字代表第几个
    * 在下面的boost::mpl::for_each<range>(boost::bind<void>(field_visitor(), boost::ref(c), v, _1));中
    * _1指代的是range生成的序列1-n用来做这个参数
    * 
    */
    struct field_visitor
    {
        template<class C, class Visitor, class I>
        void operator()(C& c, Visitor& v, I)
        {
            v(reflector::get_field_data<I::value>(c));
        }
    };

    template<class C, class Visitor>
    void visit_each_ref(C& c, Visitor& v);

    struct tuple_field_visitor
    {
        template<class C, class Visitor, class I>
        void operator()(C& c, Visitor& v, I)
        {  
            typename std::tuple_element<I::value, C>::type& new_c = *((typename std::tuple_element<I::value, C>::type*) &c);
            visit_each_ref(new_c,v);
        }
    };
    /*
     *  C是反射类，Visitor是上面的回调函数，接收参数C::field_data<N, T>就是对应的某一个反射类
     * 定义一个类似
     * struct print_visitor
     {
        template<class FieldData>
        void operator()(FieldData f)
       {
        std::cout << f.name() << "=" << f.get() << std::endl;
       }
     };的模板类来做这个遍历器
     这里使用Visitor&引用的目的是，可以使得
     struct print_visitor
     {
        int a = 0;
        template<class FieldData>
        void operator()(FieldData f)
       {
        a++;
        std::cout << f.name() << "=" << f.get() << std::endl;
       }
     };
     然后执行 print_visitor b;
     Reflector::visit_each_ref(t, b);
     结束以后b.a就是得到统计的运行次数
     当使用这个时注意要boost::mpl::for_each中使用boost::ref(v)，不然会生成副本导致失败

     boost::mpl::for_each<范围类>(),具体范围类用在哪里用占位符_1，可以用多个位置

     */
    template<class C, class Visitor>
    void visit_each_ref(C& c, Visitor& v)
    {
        if constexpr (hasfield_dataClass_v<C>)
        {
            typedef boost::mpl::range_c<size_t, 0, reflector::fields<C>::n> range;
            boost::mpl::for_each<range>(boost::bind<void>(field_visitor(), boost::ref(c), boost::ref(v), _1));
            if constexpr (hasreflectorClass_v<C>)
            {
                if constexpr (traits::is_template_of_v<std::tuple, typename C::reflectorClass>)
                {
                    typedef boost::mpl::range_c<size_t, 0, std::tuple_size<typename C::reflectorClass>::value> tupleRange;
                    typename C::reflectorClass& new_c = *((typename C::reflectorClass*) & c);
                    boost::mpl::for_each<tupleRange>(boost::bind<void>(tuple_field_visitor(), boost::ref(new_c), boost::ref(v), _1));
                }
            }
        }

    }

    //当不需要引用就是不使用局部变量的时候使用这个
    //  Reflector::visit_each_ref(t,print_visitor());
    template<class C, class Visitor>
    inline void visit_each_no_ref(C& c, Visitor v)
    {
        visit_each_ref(c, v);
    }


    template<class C, class Visitor, class = void>
    static constexpr auto fit_visit_each_ref = false;


    template<class C, class Visitor>
    static constexpr auto fit_visit_each_ref<C, Visitor, std::void_t<decltype(visit_each_ref(std::declval<C>(), std::declval<Visitor>()))>> = true;

#if defined(_CPP_20)
    template<class C, class Visitor>
    inline void visit_each(C& c, Visitor v) {
    if constexpr (requires(C & c, Visitor v) {
        visit_each_ref(c, v);
    }) {
        visit_each_ref(c, v);
    }
    else {
        visit_each_no_ref(c, v);
    }
    }
#else
    template<class C, class Visitor>
    inline void visit_each(C& c, Visitor v) {
        if constexpr (fit_visit_each_ref<C, Visitor>) {
            visit_each_ref(c, v);
        }
        else {
            visit_each_no_ref(c, v);
        }
    }
#endif


    struct field_two_visitor
    {
        template<class C, class Visitor, class I>
        void operator()(C& c1, C& c2, Visitor& v, I)
        {
            v(reflector::get_field_data<I::value>(c1), reflector::get_field_data<I::value>(c2));
        }
    };
    template<class C, class Visitor>
    void visit_two_each_ref(C& c1, C& c2, Visitor& v)
    {
        if constexpr (hasfield_dataClass_v<C>)
        {
            typedef boost::mpl::range_c<size_t, 0, reflector::fields<C>::n> range;
            boost::mpl::for_each<range>(boost::bind<void>(field_two_visitor(), boost::ref(c1), boost::ref(c2), boost::ref(v), _1));//_1代表C在field_two_visitor中的I
        }

    }
    template<class C, class Visitor>
    inline void visit_two_each_no_ref(C& c1, C& c2, Visitor v)
    {
        visit_two_each_ref(c1, c2, v);
    }

    template<class C>
    struct field_type_visitor
    {
        template<class Visitor, class I>
        void operator()(Visitor& v, I)
        {
            v.operator()<typename C::template field_data<I::value, C>>();
        }
    };

    template<class C>
    struct tuple_field_type_visitor
    {
        template<class Visitor, class I>
        void operator()(Visitor& v, I)
        {
            visit_each_type_ref<typename std::tuple_element<I::value, C::reflectorClass>::type>(v);
        }
    };
    template<class C, class Visitor>
    void visit_each_type_ref(Visitor& v)
    {
        if constexpr (hasfield_dataClass_v<C>)
        {
            typedef boost::mpl::range_c<size_t, 0, reflector::fields<C>::n> range;
            boost::mpl::for_each<range>(boost::bind<void>(field_type_visitor<C>(), boost::ref(v), _1));
            if constexpr (hasreflectorClass_v<C>)
            {
                if constexpr (traits::is_template_of_v<std::tuple, C::reflectorClass>)
                {
                    typedef boost::mpl::range_c<size_t, 0, std::tuple_size<C::reflectorClass>::value> tupleRange;
                    boost::mpl::for_each<tupleRange>(boost::bind<void>(tuple_field_type_visitor<C>(),boost::ref(v), _1));
                }
            }
        }

    }
    template<class C, class Visitor>
    inline void visit_each_type_no_ref(Visitor v)
    {
        visit_each_type_ref<C>(v);
    }

    //debug no use
    struct debug_visitor
    {
        template<class FieldData>
        void operator()(FieldData f)
        {
           // std::cout << typeid(FieldData).name() << ":" << f.name() << "=" << f.get() << std::endl;
        }
    };
    template<class C>
    inline void debug_output()
    {
        debug_visitor v;
        visit_each_type_no_ref<C>(v);
    }

    
    struct debug_type_visitor
    {
        template<class field_data>
        void operator()(field_data* t)
        {
            //std::cout << typeid(field_data::type).name() << std::endl;
        }
    };
    template<class C>
    inline void debug_output_type()
    {
        visit_each_type_no_ref<C>(debug_type_visitor());
    }

    template<typename T, typename callbackType, std::enable_if_t<Reflector::hasfield_dataClass_v<T>, int> = 0>
    void ReflectorTable(T& data, callbackType callback)
    {
        Reflector::visit_each_no_ref(data, [&]<class FieldData>(FieldData f)
        {
            callback(f.name(), typeid(f.get()), &f.get());
        });
    }
}

//要在宏定义之间加入空格只能使用宏定义否则会自动消除
#define __FORDEFINESPACE  \

#define REM(...) __VA_ARGS__  __FORDEFINESPACE \


#define EAT(...)
/*
* 
* 原来是下面这样
// Retrieve the type
#define TYPEOF(x) DETAIL_TYPEOF(DETAIL_TYPEOF_PROBE x,)
#define DETAIL_TYPEOF(...) DETAIL_TYPEOF_HEAD(__VA_ARGS__)
#define DETAIL_TYPEOF_HEAD(x, ...) REM x
#define DETAIL_TYPEOF_PROBE(...) (__VA_ARGS__),
本来的逻辑是
TYPEOF((double) x)=>DETAIL_TYPEOF(DETAIL_TYPEOF_PROBE(double) x)
                  =>DETAIL_TYPEOF((double), x)
                  =>DETAIL_TYPEOF_HEAD((double),z)
                  =>REM (double)     //，但是实际上这一步转为 REM (double),z，不知为何，这个并不是REM (double)，所以DETAIL_TYPEOF_HEAD失效
                  =>REM(double)
                  =>double

所以下面直接使用BOOST_PP_SEQ_HEAD替代DETAIL_TYPEOF_HEAD功能
*/

#define TYPEOF(x) DETAIL_TYPEOF(DETAIL_TYPEOF_PROBE x,)
#define DETAIL_TYPEOF(...) BOOST_PP_SEQ_HEAD(__VA_ARGS__)
#define DETAIL_TYPEOF_PROBE(...) (__VA_ARGS__),
// STRIP((double) p)=>EAT(double) p=>p得到变量名
#define STRIP(x) EAT x
// Show the type without parenthesis
//一般的x就是(double) p这样PAIR(x)=>REM(double) p=>double p转为定义
#define PAIR(x) REM x


/*
* self.STRIP((double) p); =》self.p
* struct testt
{
    REFLECTABLE
    (
        (const char*) name,
        (int) age
    )
};
之后
testt nn;
testt::field_data<0, testt> a(nn);

*/
#define __REFLECTABLE(...) \
static const unsigned int fields_n = BOOST_PP_VARIADIC_SIZE(__VA_ARGS__); \
friend struct Reflector::reflector; \
template<unsigned int N, class Self> \
struct field_data {}; \
BOOST_PP_SEQ_FOR_EACH_I(__REFLECT_EACH, data, BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__))

#define __REFLECT_EACH(r, data, i, x) \
PAIR(x); \
template<class Self> \
struct field_data<i, Self> \
{ \
    using type = TYPEOF(x);\
    static constexpr std::string_view variableName  = BOOST_PP_STRINGIZE(STRIP(x));\
    Self & self; \
    field_data(Self & self) : self(self) {} \
    \
    typename Reflector::make_const<Self, TYPEOF(x)>::type & get() \
    { \
        return self.STRIP(x); \
    }\
    typename boost::add_const<TYPEOF(x)>::type & get() const \
    { \
        return self.STRIP(x); \
    }\
    const char * name() const \
    {\
        return variableName.data(); \
    } \
}; \







 






/*
* 总体缺陷描述：
* 1、没有检测检测变量的重写，比如test{double a}; test1:test{int a}
*    a写了一个新的，但是遍历时依旧两份同名，不能排除同名，或者说没有能够检测出同名变量的能力，依旧执行两个同名但不同类型的输出
* 2、继承的反射类一定要用Reflector::registeredReflectorAndInherit包裹，否则就会没有遍历子类的
* 3、 REFLECTABLE中定义的变量不能狗直接 a=1这样去定义初值
    
* 
*/

/*
* //声明使用的反射类
* struct testt
{
    testt() {
        name = "testt"; age = 0, age212 = 1;
    }
    REFLECTABLE
    (
        (const char*)name,
        (int)age,
        (int)age212
    )
};
struct testtq
{
    testtq() {
        nameq = "testt"; ageq = 0;
    }
    REFLECTABLE
    (
        (const char*)nameq,
        (int)ageq
    )
};

//空类
struct testtqw
{
};

//这里即使加入testtqw这个是空类的也能正确表达
//继承后就有了  using reflectorClass ，这个属性包含了全部的类型
class testt1:public Reflector::registeredReflectorAndInherit<testt,testtq, testtqw>
{
public:
    testt1(const char* a, int b) { name1 = a; age2 = b; }
    REFLECTABLE
    (
        (const char*)name1,
        (int)age2
    )
};
 
Reflector::registeredReflectorAndInherit<testtqw, testtqw>::reflectorClass a;

testt1::field_data<0, testt1>;

constexpr auto  testvalue1 = Reflector::hasreflectorClass_v<testt1>;

struct print_visitor
{
    int a = 0;
    template<class FieldData>
    void operator()(FieldData f)
    {
        a++;
        std::cout << typeid(FieldData).name()<<":"<< f.name() << "=" << f.get() << std::endl;
    }
};

int main()
{
    testt1 t("hello",1);
    print_visitor b;
    printf("%d\r\n", &b);
    Reflector::visit_each_ref(t, b);

    return 0;
}
*/
