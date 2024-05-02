#pragma once
#include "fmath/container/complex.hpp"
#include "fmath/algorithm/matrix.hpp"
#include "fmath/algorithm/primary.hpp"
#pragma warning(disable: 4455) // for literal suffix
namespace force::math {
    template <typename Ty, std::size_t N>
    constexpr decltype(auto) operator+(const Ty& s, const basic_complex<Ty, N>& a) {
        basic_complex<Ty, N> result = a; result += s;
        return result;
    }
    template <typename Ty, std::size_t N>
    constexpr decltype(auto) operator+(const basic_complex<Ty, N>& a, const Ty& s) {
        basic_complex<Ty, N> result = a; result += s;
        return result;
    }
    template <typename Ty, std::size_t N>
    constexpr decltype(auto) operator+(const basic_complex<Ty, N>& a, const basic_complex<Ty, N>& b) {
        basic_complex<Ty, N> result = a; result += b;
        return result;
    }
    template <typename Ty, std::size_t N>
    constexpr decltype(auto) operator-(const Ty& s, const basic_complex<Ty, N>& a) {
        basic_complex<Ty, N> result = s; result -= a;
        return result;
    }
    template <typename Ty, std::size_t N>
    constexpr decltype(auto) operator-(const basic_complex<Ty, N>& a, const Ty& s) {
        basic_complex<Ty, N> result = a; result -= s;
        return result;
    }
    template <typename Ty, std::size_t N>
    constexpr decltype(auto) operator-(const basic_complex<Ty, N>& a, const basic_complex<Ty, N>& b) {
        basic_complex<Ty, N> result = a; result -= b;
        return result;
    }
    template <typename Ty, std::size_t N>
    constexpr decltype(auto) operator*(const Ty& s, const basic_complex<Ty, N>& a) {
        basic_complex<Ty, N> result = a; result *= s;
        return result;
    }
    template <typename Ty, std::size_t N>
    constexpr decltype(auto) operator*(const basic_complex<Ty, N>& a, const Ty& s) {
        basic_complex<Ty, N> result = a; result *= s;
        return result;
    }
    template <typename Ty, std::size_t N>
    constexpr decltype(auto) operator*(const basic_complex<Ty, N>& a, const basic_complex<Ty, N>& b) {
        basic_complex<Ty, N> result = a; result *= b;
        return result;
    }
    template <typename Ty, std::size_t N>
    constexpr decltype(auto) operator/(const Ty& s, const basic_complex<Ty, N>& a) {
        basic_complex<Ty, N> result = s; result /= a;
        return result;
    }
    template <typename Ty, std::size_t N>
    constexpr decltype(auto) operator/(const basic_complex<Ty, N>& a, const Ty& s) {
        basic_complex<Ty, N> result = a; result /= s;
        return result;
    }
    template <typename Ty, std::size_t N>
    constexpr decltype(auto) operator/(const basic_complex<Ty, N>& a, const basic_complex<Ty, N>& b) {
        basic_complex<Ty, N> result = a; result /= b;
        return result;
    }

    template <typename Ty, std::size_t N>
    constexpr decltype(auto) abs(const basic_complex<Ty, N>& a) {
        auto z2 = std::transform_reduce(a.begin(), a.end(), Ty(0), add, square);
        return sqrt(z2);
    }
    template <typename Ty, std::size_t N>
    constexpr decltype(auto) conjugate(const basic_complex<Ty, N>& z) {
        return basic_complex<Ty, N>(z.scalar(), -z.vector());
    }
    template <typename Ty, std::size_t N>
    constexpr decltype(auto) norm(const basic_complex<Ty, N>& z) {
        auto z2 = std::transform_reduce(z.begin(), z.end(), Ty(0), add, square);
        return rsqrt(z2) * z;
    }
    template <typename Ty, std::size_t N>
    constexpr decltype(auto) inv(const basic_complex<Ty, N>& z) {
        auto z2 = std::transform_reduce(z.begin(), z.end(), Ty(0), add, square);
        return rsqrt(z2) * conjugate(z);
    }
    template <typename Ty, std::size_t N>
    constexpr decltype(auto) exp(const basic_complex<Ty, N>& z) {
        auto v = z.vector();
        auto theta = length(v);
        return exp(z.scalar()) * basic_complex<Ty, N>{ cos(theta), (v / theta)* sin(theta)};
    }

    namespace binarion_literals {
        constexpr decltype(auto) operator""if(long double x) {
            return binarion<float>{0.F, static_cast<float>(x)};
        }
        constexpr decltype(auto) operator""if(unsigned long long x) {
            return binarion<float>{0.F, static_cast<float>(x)};
        }
        constexpr decltype(auto) operator""iF (long double x) {
            return binarion<float>{0.F, static_cast<float>(x)};
        }
        constexpr decltype(auto) operator""iF (unsigned long long x) {
            return binarion<float>{0.F, static_cast<float>(x)};
        }
        constexpr decltype(auto) operator""i(long double x) {
            return binarion<double>{0.0, static_cast<double>(x)};
        }
        constexpr decltype(auto) operator""i(unsigned long long x) {
            return binarion<double>{0.0, static_cast<double>(x)};
        }
    }
    namespace quaternion_literals {
        constexpr decltype(auto) operator""if (long double x) {
            return quaternion<float>{0.F, static_cast<float>(x), 0.F, 0.F};
        }
        constexpr decltype(auto) operator""if (unsigned long long x) {
            return quaternion<float>{0.F, static_cast<float>(x), 0.F, 0.F};
        }
        constexpr decltype(auto) operator""iF (long double x) {
            return quaternion<float>{0.F, static_cast<float>(x), 0.F, 0.F};
        }
        constexpr decltype(auto) operator""iF (unsigned long long x) {
            return quaternion<float>{0.F, static_cast<float>(x), 0.F, 0.F};
        }
        constexpr decltype(auto) operator""i(long double x) {
            return quaternion<double>{0.0, static_cast<double>(x), 0.0, 0.0};
        }
        constexpr decltype(auto) operator""i(unsigned long long x) {
            return quaternion<double>{0.0, static_cast<double>(x), 0.0, 0.0};
        }

        constexpr decltype(auto) operator""jf (long double x) {
            return quaternion<float>{0.F, 0.F, static_cast<float>(x), 0.F};
        }
        constexpr decltype(auto) operator""jf (unsigned long long x) {
            return quaternion<float>{0.F, 0.F, static_cast<float>(x), 0.F};
        }
        constexpr decltype(auto) operator""jF(long double x) {
            return quaternion<float>{0.F, 0.F, static_cast<float>(x), 0.F};
        }
        constexpr decltype(auto) operator""jF(unsigned long long x) {
            return quaternion<float>{0.F, 0.F, static_cast<float>(x), 0.F};
        }
        constexpr decltype(auto) operator""j(long double x) {
            return quaternion<double>{0.0, 0.0, static_cast<double>(x), 0.0};
        }
        constexpr decltype(auto) operator""j(unsigned long long x) {
            return quaternion<double>{0.0, 0.0, static_cast<double>(x), 0.0};
        }

        constexpr decltype(auto) operator""kf(long double x) {
            return quaternion<float>{0.F, 0.F, 0.F, static_cast<float>(x)};
        }
        constexpr decltype(auto) operator""kf(unsigned long long x) {
            return quaternion<float>{0.F, 0.F, 0.F, static_cast<float>(x)};
        }
        constexpr decltype(auto) operator""kF(long double x) {
            return quaternion<float>{0.F, 0.F, 0.F, static_cast<float>(x)};
        }
        constexpr decltype(auto) operator""kF(unsigned long long x) {
            return quaternion<float>{0.F, 0.F, 0.F, static_cast<float>(x)};
        }
        constexpr decltype(auto) operator""k(long double x) {
            return quaternion<double>{0.0, 0.0, 0.0, static_cast<double>(x)};
        }
        constexpr decltype(auto) operator""k(unsigned long long x) {
            return quaternion<double>{0.0, 0.0, 0.0, static_cast<double>(x)};
        }
    }

    
}