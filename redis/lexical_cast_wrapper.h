#pragma once
#include "type_traits.h"
#include <string>
template <typename T>
struct LexicalCastWrapper
{
    T operator() (std::string && str)
    {
        return LexicalCast<T>()(str);
    }
};

template <>
struct LexicalCastWrapper<std::string>
{
    std::string operator() (std::string && str)
    {
        return std::move(str);
    }
};
