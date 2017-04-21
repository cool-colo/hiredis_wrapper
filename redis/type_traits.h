#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <byteswap.h>
#include <cstdlib>


template<typename T>
T traits_type_default_value(T)
{
    return T();
}


template<typename T>
struct IsPodType
{
    enum { yes = 0 };
};

#define __IS_POD_TYPE(T) \
template <> \
struct IsPodType<T> \
{ \
    enum { yes = 1 }; \
};
__IS_POD_TYPE(bool);
__IS_POD_TYPE(char);
__IS_POD_TYPE(signed char);
__IS_POD_TYPE(unsigned char);
__IS_POD_TYPE(signed short);
__IS_POD_TYPE(unsigned short);
__IS_POD_TYPE(signed int);
__IS_POD_TYPE(unsigned int);
__IS_POD_TYPE(signed long);
__IS_POD_TYPE(unsigned long);
__IS_POD_TYPE(signed long long);
__IS_POD_TYPE(unsigned long long);
__IS_POD_TYPE(float);
__IS_POD_TYPE(double);


/// string to pod
template<typename T>
struct LexicalCast
{};

#define __LEXICAL_CAST_FROM_STR_TO_POD(T, STR_TO_XXX) \
template <> \
struct LexicalCast<T> \
{ \
    T operator() (const std::string& str) const \
    { \
        return STR_TO_XXX(str.c_str()); \
    } \
};
inline unsigned long long __wrap_strtoull(const char *str)
{
    return std::strtoull(str, NULL, 0);
}
inline long long __wrap_strtoll(const char *str)
{
    return std::strtoll(str, NULL, 0);
}
inline double __wrap_strtod(const char* str)
{
    return std::strtod(str, NULL);
}
__LEXICAL_CAST_FROM_STR_TO_POD(bool, __wrap_strtoull);
__LEXICAL_CAST_FROM_STR_TO_POD(char, __wrap_strtoll);
__LEXICAL_CAST_FROM_STR_TO_POD(signed char, __wrap_strtoll);
__LEXICAL_CAST_FROM_STR_TO_POD(unsigned char, __wrap_strtoull);
__LEXICAL_CAST_FROM_STR_TO_POD(signed short, __wrap_strtoll);
__LEXICAL_CAST_FROM_STR_TO_POD(unsigned short, __wrap_strtoull);
__LEXICAL_CAST_FROM_STR_TO_POD(signed int, __wrap_strtoll);
__LEXICAL_CAST_FROM_STR_TO_POD(unsigned int, __wrap_strtoull);
__LEXICAL_CAST_FROM_STR_TO_POD(signed long, __wrap_strtoll);
__LEXICAL_CAST_FROM_STR_TO_POD(unsigned long, __wrap_strtoull);
__LEXICAL_CAST_FROM_STR_TO_POD(signed long long, __wrap_strtoll);
__LEXICAL_CAST_FROM_STR_TO_POD(unsigned long long, __wrap_strtoull);
__LEXICAL_CAST_FROM_STR_TO_POD(float, __wrap_strtod);
__LEXICAL_CAST_FROM_STR_TO_POD(double, __wrap_strtod);


/// define ntohl64 & htonl64
#if BYTE_ORDER == BIG_ORDER
/* The host byte order is the same as network byte order,
   so these functions are all just identity. */
#define ntohl64(x)      (x)
#define htonl64(x)      (x)

#else //#elif BYTE_ORDER == LITTLE_ORDER
#define ntohl64(x)      bswap_64(x)
#define htonl64(x)      bswap_64(x)

#endif

/// nbits-integer traits
template <size_t NBITS>
struct NbitsIntegerTraits
{};
template <>
struct NbitsIntegerTraits<8>
{
    typedef         uint8_t     data_type;
    inline uint8_t  bswap(uint8_t val) const    { return val; }
};
template <>
struct NbitsIntegerTraits<16>
{
    typedef         uint16_t    data_type;
    inline uint16_t bswap(uint16_t val) const   { return htons(val); }
};
template <>
struct NbitsIntegerTraits<32>
{
    typedef         uint32_t    data_type;
    inline uint32_t bswap(uint32_t val) const   { return htonl(val); }
};
template <>
struct NbitsIntegerTraits<64>
{
    typedef         uint64_t    data_type;
    inline uint64_t bswap(uint64_t val) const   { return htonl64(val); }
};



