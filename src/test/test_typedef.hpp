#pragma once

// Test type traits

template<class T>
struct sink_s{typedef void type;};

template<class T>
using sink_t=typename sink_s<T>::type;

template<typename T, typename=void>
struct copy_allowed:std::false_type{};

template<typename T>
struct copy_allowed<T,sink_t<decltype(T(std::declval<T const&>()))>>:std::true_type {};

template<typename T, typename=void>
struct move_allowed:std::false_type{};

template<typename T>
struct move_allowed<T,sink_t<decltype(T(std::declval<T>()))>>:std::true_type {};

template<typename T>
struct non_copy_allowed:std::integral_constant<bool, !copy_allowed<T>::value> {};

template<typename T>
struct non_move_allowed:std::integral_constant<bool, !move_allowed<T>::value> {};

template<typename T>
struct non_move_and_copy_allowed:std::integral_constant<bool, !move_allowed<T>::value and !copy_allowed<T>::value> {};

class ctor {
    public:
        ctor() {printf("ctor %p\n", this);}
        virtual ~ctor() {printf("dtor %p\n", this);}
        ctor(const ctor &) {printf("cctor %p\n", this);}
        ctor(ctor &&) {printf("mctor %p\n", this);}
        ctor& operator=(const ctor &) {printf("cactor %p\n", this); return *this;}
        ctor& operator=(ctor &&) {printf("mactor %p\n", this); return *this;}
};