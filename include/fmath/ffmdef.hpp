#pragma once
#include <numbers>
#include <array>
#include <bit>

namespace force::math {
    using namespace ::std::numbers;

    // CONCEPTS.
    // I don't know why std library haven't provided this.
    template <typename Ty>
    concept floating_point = std::floating_point<Ty>;
    template <typename Ty>
    concept integral = std::integral<Ty>;
    template <typename Ty>
    concept arithmetic = std::floating_point<Ty> || std::integral<Ty>;
    template <std::size_t N>
    concept power_aligned = (N & (N - 1)) == 0;
    // TYPEDEFS
#if defined __STDCPP_FLOAT32_T__
    using ::std::float32_t;
#else
    typedef float    float32_t;
#endif

#if defined __STDCPP_FLOAT64_T__
    using ::std::float64_t;
#else
    typedef double   float64_t;
#endif
    typedef signed char        int8_t;
    typedef unsigned char      uint8_t;
    typedef short              int16_t;
    typedef unsigned short     uint16_t;
    typedef int                int32_t;
    typedef unsigned int       uint32_t;
    typedef long long          int64_t;
    typedef unsigned long long uint64_t;
}