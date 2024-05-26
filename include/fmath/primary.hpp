///
/// @file      primary.hpp
/// @brief     Contains basic functions that could be useful.
/// @details   ~
/// @author    HenryDu
/// @date      16.05.2024
/// @copyright © HenryDu 2024. All right reserved.
///
#pragma once
#include "fmath/ffmdef.hpp"
#include <bit> // for std::bit_cast and bit_ceil

#if FMA_USE_CMATH
#include <cmath>
#endif

#define SCALAR_DECL(RetT) [[nodsicard]] constexpr RetT
#define STATIC_SCALAR_DECL(RetT) [[nodsicard]] static constexpr RetT
namespace force::math {

    namespace detail {
        template <floating_point Ty> struct ieee_algo_constant;
        template <> struct ieee_algo_constant<float32_t> {
            typedef uint32_t correspond_int;

            static constexpr auto zero = 0.0F;
            static constexpr auto half = 0.5F;
            static constexpr auto one = 1.0F;
            static constexpr auto three_half = 1.5F;
            static constexpr auto two = 2.0F;
            static constexpr auto one_third = 0.3333333F;

            static constexpr auto mantissa_bits = 23U;
            static constexpr auto exponent_bits = 8U;
            static constexpr auto high_bit_value = 31U;

            static constexpr auto sgn_zero_mask = 0x7FFF'FFFFU;
            static constexpr auto sgn_one_mask = 0x8000'0000U;
            static constexpr auto exp_off_mask = 0x3F80'0000U; // Equal to floating point exponent bias

            static constexpr auto sqrt_magick = 0x1FBD'1DF5U;
            static constexpr auto rsqrt_magick = 0x5F37'59DFU;
            static constexpr auto cbrt_magick = 0x2A2E'5C2FU;
            static constexpr auto log_magick = 0x007F'FFFFU;

            STATIC_SCALAR_DECL(float32_t) sqrt_approximation(const float32_t x, const float32_t n) {
                auto y = x;
                y = half * y + n / y;
                y = half * y + n / y;
                return y;
            }
            STATIC_SCALAR_DECL(float32_t) rsqrt_approximation(const float32_t x, const float32_t n) {
                auto y = x;
                y = y * (three_half - n * y * y);
                y = y * (three_half - n * y * y);
                return y;
            }
            STATIC_SCALAR_DECL(float32_t) cbrt_approximation(const float32_t x, const float32_t n) {
                auto y = x;
                y = 2 * one_third * y + n * (one / (y * y));
                y = 2 * one_third * y + n * (one / (y * y));
                y = 2 * one_third * y + n * (one / (y * y));
                return y;
            }
            STATIC_SCALAR_DECL(float32_t) log_approximation(const float32_t x) {
                const float32_t x2 = x * x;
                return 2.F * (x + x2 * x * (0.333333f + x2 * (0.2f + x2 * (0.142857f + x2 * (0.111111f + x2 * 0.090909f)))));
            }
            STATIC_SCALAR_DECL(float32_t) exp_approximation(const float32_t x) {
                return 1.F + x * (1.F + x * (0.5f + x * (0.166666f + x * (0.0416666f + x * (0.0083333f + x * 0.0013888f)))));
            }
            STATIC_SCALAR_DECL(float32_t) sin_approximation(const float32_t t) {
                const float32_t h = t / 9.F;
                const float32_t h2 = h * h;
                const float32_t l = (((0.000027553F * h2 - 0.0001984F) * h2 + 0.0083333F) * h2 - 0.1666666F) * h * h2 + h;
                const float32_t j = l * (3.F - 4.F * l * l);
                return j * (3.F - 4.F * j * j);
            }
            STATIC_SCALAR_DECL(float32_t) tan_approximation(const float32_t t) {
                const float32_t h = t / 4.F;
                const float32_t h2 = h * h;
                const float32_t l = h * (1.F + h2 * (0.333333F + h2 * (0.1333333F + h2 * 0.05396825F)));
                const float32_t l2 = l * l;
                return (4.F * l * (1.F - l2)) / (1.F - 6.F * l2 + l2 * l2);
            }
            // Original from ShaderFastLibs at https://github.com/michaldrobot/ShaderFastLibs/blob/master/ShaderFastMathLib.h
            template <typename Fn>
            STATIC_SCALAR_DECL(float32_t) acos_approximation(const float32_t x, const float32_t ax, Fn fsqrt) {

                const float32_t x2 = ax * ax;
                const float32_t x3 = x2 * ax;
                float32_t s = 0.F;

                s = -0.2121144F * ax + 1.5707288F;
                s = 0.0742610F * x2 + s;
                s = -0.0187293F * x3 + s;
                s = fsqrt(1.0F - ax) * s;

                // acos function mirroring
                // check per platform if compiles to a selector - no branch needed.
                return x >= 0.0F ? s : pi_v<float32_t> -s;
            }
            template <typename Fn>
            STATIC_SCALAR_DECL(float32_t) asin_approximation(const float32_t x, const float32_t ax, Fn fsqrt) {
                return half * pi_v<float32_t> -acos_approximation(x, ax, fsqrt);
            }
            STATIC_SCALAR_DECL(float32_t) atan_approximation(const float32_t x, const float32_t ax) {
                return x * (-0.1784F * ax - 0.0663F * x * x + 1.0301F);
            }
        };
        template <> struct ieee_algo_constant<float64_t> {
            typedef uint64_t correspond_int;

            static constexpr auto zero = 0.0;
            static constexpr auto half = 0.5;
            static constexpr auto one = 1.0;
            static constexpr auto three_half = 1.5;
            static constexpr auto two = 2.0;
            static constexpr auto one_third = 0.333333333333;

            static constexpr auto mantissa_bits = 52ULL;
            static constexpr auto exponent_bits = 11ULL;
            static constexpr auto high_bit_value = 63ULL;

            static constexpr auto sgn_zero_mask = 0x7FFF'FFFF'FFFF'FFFFULL;
            static constexpr auto sgn_one_mask = 0x8000'0000'0000'0000ULL;
            static constexpr auto exp_off_mask = 0x3FF0'0000'0000'0000ULL;

            static constexpr auto log_magick = 0x000F'FFFF'FFFF'FFFFULL;
        };
    }
    // Make function implementations easier.
#define FLOAT_DETAIL_ALIAS(n) using Constants=::force::math::detail::ieee_algo_constant<n>; using Int = Constants::correspond_int
    /////////////////////////////////////////////////////////////////
    ///        FLOATING-POINT OPTIMIZED FUNCTIONS(UNARY/BINARY)   ///
    /////////////////////////////////////////////////////////////////
    template <floating_point Float>
    SCALAR_DECL(Float) inv(const Float x) {
        FLOAT_DETAIL_ALIAS(Float);
        return Constants::one / x;
    }
    template <floating_point Float>
    SCALAR_DECL(Float) sgn(const Float x) {
        FLOAT_DETAIL_ALIAS(Float);
        auto i = std::bit_cast<Int>(x);
        auto f = std::bit_cast<Float>((i & Constants::sgn_one_mask) | Constants::exp_off_mask);
        return f;
    }
    template <floating_point Float>
    SCALAR_DECL(Float) trunc(const Float x) {
        FLOAT_DETAIL_ALIAS(Float);
        auto i = Int(x);
        return Float(i);
    }
    template <floating_point Float>
    SCALAR_DECL(Float) mod(const Float n, const Float d) {
        FLOAT_DETAIL_ALIAS(Float);
        auto f = n / d;
        return (f - trunc(f)) * d;
    }
    template <floating_point Float>
    SCALAR_DECL(Float) ceil(const Float x) {
        FLOAT_DETAIL_ALIAS(Float);
        return Int(x + Constants::half);
    }
    template <floating_point Float>
    SCALAR_DECL(Float) floor(const Float x) {
        FLOAT_DETAIL_ALIAS(Float);
        return Int(x - Constants::half);
    }
    template <floating_point Float>
    SCALAR_DECL(Float) round(const Float x) {
        FLOAT_DETAIL_ALIAS(Float);
        return Int(x + Constants::half * sgn(x));
    }
    template <floating_point Float>
    SCALAR_DECL(Float) banker_round(const Float x) {
        FLOAT_DETAIL_ALIAS(Float);
        auto k = Int(x) & 1;
        return x + sgn(x) * (Float(k) - Constants::half);
    }
    template <floating_point Float>
    SCALAR_DECL(Float) abs(const Float x) {
        FLOAT_DETAIL_ALIAS(Float);
        auto i = std::bit_cast<Int>(x);
        i &= Constants::sgn_zero_mask;
        return std::bit_cast<Float>(i);
    }
    template <floating_point Float>
    SCALAR_DECL(Float) sqrt(const Float x) {
        FLOAT_DETAIL_ALIAS(Float);
#if FMA_USE_STD_SQRT
        return std::sqrt(x);
#else
        Float n = Constants::half * x;
        auto  i = std::bit_cast<Int>(x);
        i = Constants::sqrt_magick + (i >> 1);
        auto  y = std::bit_cast<Float>(i);
        // approximation.
        return Constants::sqrt_approximation(y, n);
#endif
    }
    template <floating_point Float>
    SCALAR_DECL(Float) rsqrt(const Float x) {
        FLOAT_DETAIL_ALIAS(Float);
#if FMA_USE_STD_SQRT
        return Constants::one / std::sqrt(x);
#else
        Float n = Constants::half * x;
        auto  i = std::bit_cast<Int>(x);
        i = Constants::rsqrt_magick - (i >> 1); // wtf, this is better?
        auto  y = std::bit_cast<Float>(i);
        // approximation.
        return Constants::rsqrt_approximation(y, n);
#endif

    }
    template <floating_point Float>
    SCALAR_DECL(Float) cbrt(const Float x) {
        FLOAT_DETAIL_ALIAS(Float);
#if FMA_USE_STD_CBRT
        return std::cbrt(x);
#else
        auto   z = abs(x);
        Float  n = Constants::one_third * z;
        auto   i = std::bit_cast<Int>(z);
        i = Constants::cbrt_magick + (i / 3);
        auto   y = std::bit_cast<Float>(i);
        // Approximation.
        y = Constants::cbrt_approximation(y);
        // Cube root function has f(-x) = -f(x).
        return x > Constants::zero ? y : (-y);
#endif
    }
    template <floating_point Float>
    SCALAR_DECL(Float) log(const Float x) {
        FLOAT_DETAIL_ALIAS(Float);
#if FMA_USE_STD_LOG
        return ::std::log(x);
#else
        Int  i = std::bit_cast<int32_t>(x);
        Int  e = (i >> Constants::mantissa_bits) - (1 << (Constants::exponent_bits - 1)) + 1;
        Int  f = (i & Constants::log_magick) | Constants::exp_off_mask;
        auto m = std::bit_cast<Float>(f);
        // Approximation using log((m-1)/(m+1))
        Float t = (m - Constants::one) / (m + Constants::one);
        return ln2_v<Float> *Float(e) + Constants::log_approximation(t);
#endif
    }
    template <floating_point Float>
    SCALAR_DECL(Float) log2(const Float x) {
#if FMA_USE_STD_LOG2
        return ::std::log2(x);
#else
        return log2e_v<Float>*log(x);
#endif
    }
    template <floating_point Float>
    SCALAR_DECL(Float) log10(const Float x) {
#if FMA_USE_STD_LOG10
        return ::Std::log10(x);
#else
        return log10e_v<Float>*log(x);
#endif
    }
    template <floating_point Float>
    SCALAR_DECL(Float) exp(const Float x) {
        FLOAT_DETAIL_ALIAS(Float);
#if FMA_USE_STD_EXP
        return ::std::exp(x);
#else
        Float t = x * log2e_v<Float>;
        auto  i = Int(trunc(t));
        Float f = t - Float(i);
        Int   a = (i + (1 << (Constants::exponent_bits - 1)) - 1) << Constants::mantissa_bits;
        Float b = ln2_v<Float> *f; // 2^f = e^(ln2 * f) = e^b
        // Approximation.
        return std::bit_cast<Float>(a) * Constants::exp_approximation(b);
#endif
    }
    template <floating_point Float>
    SCALAR_DECL(Float) exp2(const Float x) {
#if FMA_USE_STD_EXP2
        return ::std::exp2(x);
#else
        return exp(ln2_v<Float> *x);
#endif
    }
    template <floating_point Float>
    SCALAR_DECL(Float) exp10(const Float x) {
#if FMA_USE_STD_EXP10
        return ::std::exp10(x);
#else
        return exp(ln10_v<Float> *x);
#endif
    }
    template <floating_point Float>
    SCALAR_DECL(Float) sin(const Float x) {
#ifdef FMA_USE_STD_SIN
        return ::std::sin(x);
#else
        FLOAT_DETAIL_ALIAS(Float);
        Float k = abs(x);
        Float f = Constants::two * inv_pi_v<Float> * k;           // Fraction is the result.
        Int   q = Int(f);                                         // Quotient is int32_t part of f.
        Float r = (f - Float(q)) * Constants::half * pi_v<Float>; // Remainder is a the angle remainded.
        Int   d = q & 3;                                          // q mod 4.
        Float a = abs(Constants::three_half - Float(d));
        Int   e = Int(a + Constants::one);                        // Translate sign's exponent
        Float m = (r - (d & 1) * Constants::half * pi_v<Float>);  // Translate angle.
        Int   p = ((e & 1) << Constants::high_bit_value) | Constants::exp_off_mask;
        Float t = std::bit_cast<Float>(p) * m;                // theta is the angle translated between [-pi/2, pi/2].
        // Approximation.
        auto  y = Constants::sin_approximation(t);
        return x > Constants::zero ? y : (-y);
#endif
    }
    template <floating_point Float>
    SCALAR_DECL(Float) pow(const Float x, const Float y) {
#ifdef FMA_USE_STD_POW
        return ::std::pow(x, y);
#else
        return exp(y * log(x));
#endif
    }
    template <floating_point Float>
    SCALAR_DECL(Float) cos(const Float x) {
#ifdef FMA_USE_STD_COS
        return ::std::cos(x);
#else
        FLOAT_DETAIL_ALIAS(Float);
        return sin(Constants::half * pi_v<Float> -x);
#endif
    }
    template <floating_point Float>
    SCALAR_DECL(Float) tan(const Float x) {
#if FMA_USE_STD_TAN
        return ::std::tan(x);
#else
        FLOAT_DETAIL_ALIAS(Float);
        Float v = abs(x);
        Float f = Constants::two * x * inv_pi_v<Float>;
        Int   q = Int(f);
        Float r = (f - Float(q)) * Constants::half * pi_v<Float>;
        Int   d = q & 1;
        Float t = r - Float(d) * Constants::half * pi_v<Float>;
        // Approximation.
        auto  y = Constants::tan_approximation(t);
        return x > 0 ? y : (-y);
#endif
    }
    template <floating_point Float>
    SCALAR_DECL(Float) cot(const Float x) {
        FLOAT_DETAIL_ALIAS(Float);
        return Constants::one / tan(x);
    }
    template <floating_point Float>
    SCALAR_DECL(Float) sec(const Float x) {
        FLOAT_DETAIL_ALIAS(Float);
        return Constants::one / cos(x);
    }
    template <floating_point Float>
    SCALAR_DECL(Float) csc(const Float x) {
        FLOAT_DETAIL_ALIAS(Float);
        return Constants::one / sin(x);
    }
    template <floating_point Float>
    SCALAR_DECL(Float) asin(const Float x) {
#if FMA_USE_STD_ASIN
        return ::std::asin(x);
#else
        FLOAT_DETAIL_ALIAS(Float);
        return Constants::asin_approximation(x, abs(x), sqrt<Float>);
#endif
    }
    template <floating_point Float>
    SCALAR_DECL(Float) acos(const Float x) {
#if FMA_USE_STD_ACOS
        return ::std::acos(x);
#else
        FLOAT_DETAIL_ALIAS(Float);
        return Constants::acos_approximation(x, abs(x), sqrt<Float>);
#endif
    }
    template <floating_point Float>
    SCALAR_DECL(Float) atan(const Float x) {
#if FMA_USE_STD_ATAN
        return ::std::atan(x);
#else
        FLOAT_DETAIL_ALIAS(Float);
        return Constants::atan_approximation(x, abs(x));
#endif
    }
    template <floating_point Float>
    SCALAR_DECL(Float) acot(const Float x) {
        FLOAT_DETAIL_ALIAS(Float);
        return Constants::half * pi_v<Float> -atan(x);
    }
    template <floating_point Float>
    SCALAR_DECL(Float) asec(const Float x) {
        return acos(inv(x));
    }
    template <floating_point Float>
    SCALAR_DECL(Float) acsc(const Float x) {
        return asin(inv(x));
    }

    template <floating_point Float>
    SCALAR_DECL(Float) sinh(const Float x) {
#if FMA_USE_STD_SINH
        return ::std::sinh(x);
#else
        FLOAT_DETAIL_ALIAS(Float);
        return (exp(x) - exp(-x)) * Constants::half;
#endif
    }
    template <floating_point Float>
    SCALAR_DECL(Float) cosh(const Float x) {
#if FMA_USE_STD_COSH
        return ::std::cosh(x);
#else
        FLOAT_DETAIL_ALIAS(Float);
        return (exp(x) + exp((-x))) * Constants::half;
#endif
    }
    template <floating_point Float>
    SCALAR_DECL(Float) tanh(const Float x) {
#if FMA_USE_STD_TANH
        return ::std::tanh(x);
#else
        FLOAT_DETAIL_ALIAS(Float);
        return (exp(x) - exp((-x))) / (exp(x) + exp((-x)));
#endif
    }
    template <floating_point Float>
    SCALAR_DECL(Float) coth(const Float x) {
        return (exp(x) + exp((-x))) / (exp(x) - exp((-x)));
    }
    template <floating_point Float>
    SCALAR_DECL(Float) sech(const Float x) {
        FLOAT_DETAIL_ALIAS(Float);
        return Constants::two / (exp(x) + exp((-x)));
    }
    template <floating_point Float>
    SCALAR_DECL(Float) csch(const Float x) {
        FLOAT_DETAIL_ALIAS(Float);
        return Constants::two / (exp(x) - exp((-x)));
    }
    template <floating_point Float>
    SCALAR_DECL(Float) asinh(const Float x) {
#if FMA_USE_STD_SINH
        return ::std::asinh(x);
#else
        FLOAT_DETAIL_ALIAS(Float);
        return log(x + sqrt(x * x + Constants::one));
#endif
    }
    template <floating_point Float>
    SCALAR_DECL(Float) acosh(const Float x) {
#if FMA_USE_STD_SINH
        return ::std::acosh(x);
#else
        FLOAT_DETAIL_ALIAS(Float);
        return log(x + sqrt(x * x - Constants::one));
#endif
    }
    template <floating_point Float>
    SCALAR_DECL(Float) atanh(const Float x) {
#if FMA_USE_STD_SINH
        return ::std::atanh(x);
#else
        FLOAT_DETAIL_ALIAS(Float);
        return Constants::half * log((Constants::one + x) / (Constants::one - x));
#endif
    }
    template <floating_point Float>
    SCALAR_DECL(Float) acoth(const Float x) {
        FLOAT_DETAIL_ALIAS(Float);
        return Constants::half * log((x + Constants::one) / (x - Constants::one));
    }
    template <floating_point Float>
    SCALAR_DECL(Float) asech(const Float x) {
        FLOAT_DETAIL_ALIAS(Float);
        return log(Constants::one + sqrt(Constants::one - x * x)) - log(x);
    }
    template <floating_point Float>
    SCALAR_DECL(Float) acsch(const Float x) {
        FLOAT_DETAIL_ALIAS(Float);
        return log(inv(x) + sqrt(Constants::one + x * x) / abs(x));
    }


    /////////////////////////////////////////
    ///    INTEGRAL OPTIMIZED FUNCTIONS   ///
    /////////////////////////////////////////
    template <integral Int>
    SCALAR_DECL(Int) abs(const Int x) {
        if constexpr (std::is_unsigned_v<Int>) {
            return Int(x);
        }
        constexpr auto m = (sizeof(Int) << 3) - 1;
        return ((x >> m) ^ x) - (x >> m);
    }
    template <integral Int>
    SCALAR_DECL(Int) mod(const Int a, const Int b) {
        return a % b;
    }
    template <integral Int>
    SCALAR_DECL(Int) log2(const Int x) {
        auto y = static_cast<std::make_unsigned_t<Int>>(x);
        return (sizeof(Int) << 3) - std::countl_zero(y) - 1;
    }
    template <integral Int>
    SCALAR_DECL(Int) exp2(const Int x) {
        return x > 0 ? (1 << x) : (1 >> (-x));
    }
    /////////////////////////////////////////
    ///         HELPER FUNCTIONS          ///
    /////////////////////////////////////////
    template <typename Ty>
    SCALAR_DECL(Ty) rad(Ty deg) {
        return deg * (static_cast<Ty>(180) / pi_v<Ty>);
    }
    template <typename Ty>
    SCALAR_DECL(Ty) deg(Ty rad) {
        return rad * (pi_v<Ty> / static_cast<Ty>(180));
    }
    template <typename Ty>
    SCALAR_DECL(Ty) clamp(Ty v, Ty min, Ty max) {
#if FMA_USE_STD_LERP
        return std::clamp(v, min, max);
#else
        return v < min ? min : v > max ? max : v;
#endif
    }
    // Exact lerp function guarantee the bound case of 't = 1' v0 = v1.
    // However, it might lose some speed, for faster and fma optimize version
    // Use lerp_ac -- lerp accelerated.
    template <std::floating_point Float>
    SCALAR_DECL(Float) lerp(Float v0, Float v1, Float t) {
#if FMA_USE_STD_LERP
        return std::lerp(v0, v1, t);
#else
        return (1 - t) * v0 + t * v1;
#endif
    }
    // Accelerated lerp but not guarantee v0 = v1 when t = 1.
    template <std::floating_point Float>
    SCALAR_DECL(Float) lerp_ac(Float v0, Float v1, Float t) {
        return v0 + t * (v1 - v0);
    }
    template <typename Ty>
    SCALAR_DECL(Ty) midpoint(Ty v0, Ty v1) {
#if FMA_USE_STD_MIDPOINT
        return std::midpoint(v0, v1);
#else
        return v0 + ((v1 - v0) >> 1); // Pointers also can use this way.
#endif
    }

    ///////////////////////////////////////////
    ///         UNARY OPTION FUNCTIONS      ///
    ///////////////////////////////////////////
    struct square_proto {
        template <typename Ty>
        constexpr decltype(auto) operator()(const Ty x) const { return x * x; }
    };
    constexpr auto square = square_proto{};
    struct cube_proto {
        template <typename Ty>
        constexpr decltype(auto) operator()(const Ty x) const { return x * x * x; }
    };
    constexpr auto cube = cube_proto{};
    ////////////////////////////////////////////
    ///         BINARY OPTION FUNCTIONS      ///
    ////////////////////////////////////////////
    struct add_proto {
        template <typename Ty>
        constexpr decltype(auto) operator()(const Ty x, const Ty y) const { return x + y; }
    };
    constexpr auto add = add_proto{};
    struct sub_proto {
        template <typename Ty>
        constexpr decltype(auto) operator()(const Ty x, const Ty y) const { return x - y; }
    };
    constexpr auto sub = sub_proto{};
    struct mul_proto {
        template <typename Ty>
        constexpr decltype(auto) operator()(const Ty x, const Ty y) const { return x * y; }
    };
    constexpr auto mul = mul_proto{};
    struct div_proto {
        template <typename Ty>
        constexpr decltype(auto) operator()(const Ty x, const Ty y) const { return x / y; }
    };
    constexpr auto div = div_proto{};
#undef SCALAR_DECL
#undef STATIC_SCALAR_DECL
#undef FLOAT_DETAIL_ALIAS
}
