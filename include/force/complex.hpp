///
/// @file      complex.hpp
/// @brief     Generalized complex class (for all 2^n dimensional complexes)
/// @details   ~
/// @author    HenryDu
/// @date      16.05.2024
/// @copyright © HenryDu 2024. All right reserved.
///
#pragma once
#include "force/vector.hpp"
#ifdef _MSC_VER
    #pragma warning(disable: 4455) // for literal suffix
#endif

namespace force {
    // This is a generic version of complex number try not to mess it up with std::complex.
    template <arithmetic Ty, std::size_t N> requires ((N & (N - 1)) == 0)
    class complex {
    public:
        using value_type             = Ty;
        using vector_type            = vector<value_type, N - 1>;
        using pointer                = Ty*;
        using reference              = Ty&;
        using const_reference        = const Ty&;
        using const_pointer          = const Ty*;
        using iterator               = pointer;
        using const_iterator         = const_pointer;
        using reverse_iterator       = std::reverse_iterator<iterator>;
        using const_reverse_iterator = const std::reverse_iterator<iterator>;

        constexpr complex()                                   noexcept = default;
        constexpr complex(const Ty& a)                        noexcept : mScalar(a), mVector() {}
        constexpr complex(std::initializer_list<Ty> l)        noexcept { std::copy_n(l.begin(), N, &mScalar); }
        constexpr complex(const Ty& a, const vector_type& v)  noexcept : mScalar(a), mVector(v) {}

        constexpr complex(const complex&) noexcept = default;
        constexpr complex(complex&&)      noexcept = default;

        constexpr complex& operator=(const complex&) noexcept = default;
        constexpr complex& operator=(complex&&)      noexcept = default;

        constexpr reference       operator[](const std::ptrdiff_t i)       { return (&mScalar)[i]; }
        constexpr const_reference operator[](const std::ptrdiff_t i) const { return (&mScalar)[i]; }

        constexpr reference          scalar()       { return mScalar; }
        constexpr const_reference    scalar() const { return mScalar; }
        constexpr vector_type&       vector()       { return mVector; }
        constexpr const vector_type& vector() const { return mVector; }

        constexpr pointer            data()         { return &mScalar; }
        constexpr const_pointer      data()   const { return &mScalar; }


        constexpr iterator           begin()       { return &mScalar; }
        constexpr iterator           end()         { return (&mScalar) + N; }
        constexpr const_iterator     begin() const { return &mScalar; }
        constexpr const_iterator     end()   const { return (&mScalar) + N; }

        constexpr reverse_iterator       rbegin()       { return reverse_iterator(end()); }
        constexpr reverse_iterator       rend()         { return reverse_iterator(begin()); }
        constexpr const_reverse_iterator rbegin() const { return reverse_iterator(end()); }
        constexpr const_reverse_iterator rend()   const { return reverse_iterator(begin()); }

        constexpr const_iterator         cbegin()  const { return &mScalar; }
        constexpr const_iterator         cend()    const { return (&mScalar) + N; }
        constexpr const_reverse_iterator crbegin() const { return reverse_iterator(end()); }
        constexpr const_reverse_iterator crend()   const { return reverse_iterator(begin()); }

        constexpr std::size_t            size()    const { return N; }

        constexpr complex& operator+=(const value_type rhs) {
            mScalar += rhs;
            return *this;
        }
        constexpr complex& operator-=(const value_type rhs) {
            mScalar -= rhs;
            return *this;
        }
        constexpr complex& operator+=(const complex& rhs) {
            for (std::size_t i = 0; i != N; ++i) (*this)[i] += rhs[i];
            return *this;
        }
        constexpr complex& operator-=(const complex& rhs) {
            for (std::size_t i = 0; i != N; ++i) (*this)[i] -= rhs[i];
            return *this;
        }
        constexpr complex& operator*=(const value_type rhs) {
            for (std::size_t i = 0; i != N; ++i) (*this)[i] *= rhs;
            return *this;
        }
        constexpr complex& operator*=(const complex& rhs) {
            auto s = mScalar * rhs.mScalar - mVector.dot(rhs.mVector);
            auto v = rhs.mScalar * mVector + mScalar * rhs.mVector + rhs.mVector.cross(mVector);
            *this = complex(s, v);
            return *this;
        }
        constexpr complex& operator/=(const complex& rhs) {
            auto s = mScalar * rhs.mScalar + mVector.dot(rhs.mVector);
            auto v = rhs.mScalar * mVector - mScalar * rhs.mVector + mVector.cross(rhs.mVector);
            *this = rsqrt(std::transform_reduce(rhs.begin(), rhs.end(), Ty(0), add, square)) * complex(s, v);
            return *this;
        }
        constexpr complex& operator/=(const value_type rhs) {
            for (std::size_t i = 0; i != N; ++i) (*this)[i] /= rhs;
            return *this;
        }
        constexpr const value_type dot(const complex& rhs) const {
            return std::transform_reduce(begin(), end(), rhs.begin(), Ty(0), add, mul);
        }
        constexpr decltype(auto) conjugate() const {
            return complex(scalar(), vector());
        }
        constexpr decltype(auto) normalize() const {
            auto z2 = std::transform_reduce(begin(), end(), Ty(0), add, square);
            return rsqrt(z2) * (*this);
        }

        ~complex() = default;
    protected:
        Ty mScalar; vector_type mVector;
    };

    template <typename Ty> using binaron    = complex<Ty, 2>;
    template <typename Ty> using quaternion = complex<Ty, 4>;

    template <typename Ty, std::size_t N>
    constexpr decltype(auto) operator+(const Ty& s, const complex<Ty, N>& a) {
        complex<Ty, N> result = a; result += s;
        return result;
    }
    template <typename Ty, std::size_t N>
    constexpr decltype(auto) operator+(const complex<Ty, N>& a, const Ty& s) {
        complex<Ty, N> result = a; result += s;
        return result;
    }
    template <typename Ty, std::size_t N>
    constexpr decltype(auto) operator+(const complex<Ty, N>& a, const complex<Ty, N>& b) {
        complex<Ty, N> result = a; result += b;
        return result;
    }
    template <typename Ty, std::size_t N>
    constexpr decltype(auto) operator-(const Ty& s, const complex<Ty, N>& a) {
        complex<Ty, N> result = s; result -= a;
        return result;
    }
    template <typename Ty, std::size_t N>
    constexpr decltype(auto) operator-(const complex<Ty, N>& a, const Ty& s) {
        complex<Ty, N> result = a; result -= s;
        return result;
    }
    template <typename Ty, std::size_t N>
    constexpr decltype(auto) operator-(const complex<Ty, N>& a, const complex<Ty, N>& b) {
        complex<Ty, N> result = a; result -= b;
        return result;
    }
    template <typename Ty, std::size_t N>
    constexpr decltype(auto) operator*(const Ty& s, const complex<Ty, N>& a) {
        complex<Ty, N> result = a; result *= s;
        return result;
    }
    template <typename Ty, std::size_t N>
    constexpr decltype(auto) operator*(const complex<Ty, N>& a, const Ty& s) {
        complex<Ty, N> result = a; result *= s;
        return result;
    }
    template <typename Ty, std::size_t N>
    constexpr decltype(auto) operator*(const complex<Ty, N>& a, const complex<Ty, N>& b) {
        complex<Ty, N> result = a; result *= b;
        return result;
    }
    template <typename Ty, std::size_t N>
    constexpr decltype(auto) operator/(const Ty& s, const complex<Ty, N>& a) {
        complex<Ty, N> result = s; result /= a;
        return result;
    }
    template <typename Ty, std::size_t N>
    constexpr decltype(auto) operator/(const complex<Ty, N>& a, const Ty& s) {
        complex<Ty, N> result = a; result /= s;
        return result;
    }
    template <typename Ty, std::size_t N>
    constexpr decltype(auto) operator/(const complex<Ty, N>& a, const complex<Ty, N>& b) {
        complex<Ty, N> result = a; result /= b;
        return result;
    }
    // Complex algorithms.
    template <typename Ty, std::size_t N>
    constexpr decltype(auto) abs(const complex<Ty, N>& a) {
        auto z2 = std::transform_reduce(a.begin(), a.end(), Ty(0), add, square);
        return sqrt(z2);
    }
    template <typename Ty, std::size_t N>
    constexpr decltype(auto) neg(const complex<Ty, N>& z) {
        auto z2 = std::transform_reduce(z.begin(), z.end(), Ty(0), add, square);
        return rsqrt(z2) * z.conjugate();
    }
    template <typename Ty, std::size_t N>
    constexpr decltype(auto) exp(const complex<Ty, N>& z) {
        auto theta = abs(z.vector());
        return exp(z.scalar()) * complex<Ty, N>{ cos(theta), z.vector() * (sin(theta) / theta)};
    }
    // Notice binaron and quaternion can't use suffix the same time!
    // By the way I think we might not use binaron and quaternion the same time.
    namespace binaron_literals {
        constexpr decltype(auto) operator""if (long double x)         { return binaron<float>{0.F, static_cast<float>(x)};   }
        constexpr decltype(auto) operator""if (unsigned long long x)  { return binaron<float>{0.F, static_cast<float>(x)};   }
        constexpr decltype(auto) operator""iF (long double x)         { return binaron<float>{0.F, static_cast<float>(x)};   }
        constexpr decltype(auto) operator""iF (unsigned long long x)  { return binaron<float>{0.F, static_cast<float>(x)};   }
        constexpr decltype(auto) operator""i  (long double x)         { return binaron<double>{0.0, static_cast<double>(x)}; }
        constexpr decltype(auto) operator""i  (unsigned long long x)  { return binaron<double>{0.0, static_cast<double>(x)}; }
    }
    namespace quaternion_literals {
        constexpr decltype(auto) operator""if (long double x)        { return quaternion<float>{0.F, static_cast<float>(x), 0.F, 0.F};   }
        constexpr decltype(auto) operator""if (unsigned long long x) { return quaternion<float>{0.F, static_cast<float>(x), 0.F, 0.F};   }
        constexpr decltype(auto) operator""iF (long double x)        { return quaternion<float>{0.F, static_cast<float>(x), 0.F, 0.F};   }
        constexpr decltype(auto) operator""iF (unsigned long long x) { return quaternion<float>{0.F, static_cast<float>(x), 0.F, 0.F};   }
        constexpr decltype(auto) operator""i  (long double x)        { return quaternion<double>{0.0, static_cast<double>(x), 0.0, 0.0}; }
        constexpr decltype(auto) operator""i  (unsigned long long x) { return quaternion<double>{0.0, static_cast<double>(x), 0.0, 0.0}; }
        constexpr decltype(auto) operator""jf (long double x)        { return quaternion<float>{0.F, 0.F, static_cast<float>(x), 0.F};   }
        constexpr decltype(auto) operator""jf (unsigned long long x) { return quaternion<float>{0.F, 0.F, static_cast<float>(x), 0.F};   }
        constexpr decltype(auto) operator""jF (long double x)        { return quaternion<float>{0.F, 0.F, static_cast<float>(x), 0.F};   }
        constexpr decltype(auto) operator""jF (unsigned long long x) { return quaternion<float>{0.F, 0.F, static_cast<float>(x), 0.F};   }
        constexpr decltype(auto) operator""j  (long double x)        { return quaternion<double>{0.0, 0.0, static_cast<double>(x), 0.0}; }
        constexpr decltype(auto) operator""j  (unsigned long long x) { return quaternion<double>{0.0, 0.0, static_cast<double>(x), 0.0}; }
        constexpr decltype(auto) operator""kf (long double x)        { return quaternion<float>{0.F, 0.F, 0.F, static_cast<float>(x)};   }
        constexpr decltype(auto) operator""kf (unsigned long long x) { return quaternion<float>{0.F, 0.F, 0.F, static_cast<float>(x)};   }
        constexpr decltype(auto) operator""kF (long double x)        { return quaternion<float>{0.F, 0.F, 0.F, static_cast<float>(x)};   }
        constexpr decltype(auto) operator""kF (unsigned long long x) { return quaternion<float>{0.F, 0.F, 0.F, static_cast<float>(x)};   }
        constexpr decltype(auto) operator""k  (long double x)        { return quaternion<double>{0.0, 0.0, 0.0, static_cast<double>(x)}; }
        constexpr decltype(auto) operator""k  (unsigned long long x) { return quaternion<double>{0.0, 0.0, 0.0, static_cast<double>(x)}; }
    }
}