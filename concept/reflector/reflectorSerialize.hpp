#pragma once

#include "concept/reflector/reflector.hpp"
#include <cereal/types/unordered_map.hpp>
#include <cereal/types/memory.hpp>
#include <cereal/archives/binary.hpp>
#include <cereal/archives/json.hpp>
#include <boost/preprocessor/seq/for_each.hpp>
#include <boost/preprocessor/seq/for_each.hpp>
#include <boost/preprocessor/seq/to_tuple.hpp>
#include <boost/preprocessor/stringize.hpp>

#define __reflectorSerializeEquation(reflectorClass)\
bool operator==(const reflectorClass& other)const\
{\
    bool ret = true;\
    Reflector::visit_two_each_no_ref(*this, other, [&](auto fieldData1, auto fieldData2)\
        {\
            ret &= (fieldData1.get() == fieldData2.get());\
        });\
    return ret;\
}

#define __reflectorSerializeRegistration(...) \
    template <class Archive>\
    void serialize(Archive& ar)\
    {\
        \
  ar BOOST_PP_SEQ_TO_TUPLE(BOOST_PP_SEQ_FOR_EACH_I(____reflectorSerializeRegistration_EACH,data, BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__))) ;\
  }

#define ____reflectorSerializeRegistration_EACH(r, data,i,x)  (CEREAL_NVP_(BOOST_PP_STRINGIZE(STRIP(x)),STRIP(x)))

#define __reflectorWithSerialize(...) \
         __REFLECTABLE(__VA_ARGS__)\
         __reflectorSerializeRegistration(__VA_ARGS__)   

#include <fstream>
#include <iostream>
template<typename T>
concept SerializeStruct = requires(T & r, cereal::JSONOutputArchive archive) {
    archive(r);
};

template<typename T>
concept ReflectorSerializeStruct = Reflector::ReflectorClass<T>&&SerializeStruct<T>;

namespace Reflector
{
    template<typename T>
    requires ReflectorSerializeStruct<T>
    static auto to_string(T& data)
    {
        std::stringstream ss;
        {
            cereal::JSONOutputArchive archive(ss);
            archive(data);
        }
        return  ss.str();
    }
}
