#pragma once
#ifndef __cplusplus
    // bring the C++ some operator definitions into C
    // https://gcc.gnu.org/onlinedocs/cpp/C_002b_002b-Named-Operators.html#C_002b_002b-Named-Operators
    #include <iso646.h>
    #define not !
    #define and &&
    #define or ||

    #define __cfunc

    #define null NULL

#else

    #ifdef __GNUC__
        #define __cfunc extern "C"
    #endif

    // defines operator* for std::string, short alias for string.c_str()
    #include <string>
    inline const char* operator*(const std::string& s) { return s.c_str(); }

    // BOOST_STRONG_TYPEDEF, to use without boost
    // avoid implicit type cast conversion
    #define TYPEDEF(T, D)                                                  \
        struct D {                                                         \
                T t;                                                       \
                explicit D(const T t_) : t(t_){};                          \
                D(){};                                                     \
                D(const D& t_) : t(t_.t) {}                                \
                D& operator=(const D& rhs) {                               \
                    t = rhs.t;                                             \
                    return *this;                                          \
                }                                                          \
                D& operator=(const T& rhs) {                               \
                    t = rhs;                                               \
                    return *this;                                          \
                }                                                          \
                    operator const T&() const { return t; }               \
                    operator T&() { return t; }                           \
                bool operator==(const D& rhs) const { return t == rhs.t; } \
                bool operator<(const D& rhs) const { return t < rhs.t; }   \
        }

#endif

typedef enum { error = 0, ok } return_e;
typedef enum { success = 0, failure } exit_e;

#define unless(x) if (not(x))
