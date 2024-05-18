///
/// @file      complex.hpp
/// @brief     Generalized complex class (for all 2^n dimensional complexes)
/// @details   ~
/// @author    HenryDu
/// @date      16.05.2024
/// @copyright © HenryDu 2024. All right reserved.
///
#pragma once
#include "fmath/basic_matrix.hpp"
#pragma warning(disable: 4455) // for literal suffix
namespace force::math {
    template <arithmetic Ty, std::size_t N> requires ((N & (N - 1)) == 0)
    class basic_complex {
    public:
        using value_type             = Ty;
        using vector_type            = basic_matrix<value_type, 1, N - 1>;
        using pointer                = Ty*;
        using reference              = Ty&;
        using const_reference        = const Ty&;
        using const_pointer          = const Ty*;
        using iterator               = pointer;
        using const_iterator         = const_pointer;
        using reverse_iterator       = std::reverse_iterator<iterator>;
        using const_reverse_iterator = const std::reverse_iterator<iterator>;

        constexpr basic_complex()                                   noexcept = default;
        constexpr basic_complex(const Ty& a)                        noexcept : mScalar(a), mVector() {}
        constexpr basic_complex(std::initializer_list<Ty> l)        noexcept { std::copy_n(l.begin(), N, &mScalar); }
        constexpr basic_complex(const Ty& a, const vector_type& v)  noexcept : mScalar(a), mVector(v) {}

        constexpr basic_complex(const basic_complex&) noexcept = default;
        constexpr basic_complex(basic_complex&&)      noexcept = default;

        constexpr basic_complex& operator=(const basic_complex&) noexcept = default;
        constexpr basic_complex& operator=(basic_complex&&)      noexcept = default;

        constexpr reference       operator[](const std::ptrdiff_t i)       { return (&mScalar)[i]; }
        constexpr const_reference operator[](const std::ptrdiff_t i) const { return (&mScalar)[i]; }

        constexpr reference          scalar()       { return mScalar; }
        constexpr const_reference    scalar() const { return mScalar; }
        constexpr vector_type&       vector()       { return mVector; }
        constexpr const vector_type& vector() const { return mVector; }

        constexpr pointer            data()         { return &mScalar; }
        constexpr const_pointer      data()   const { return &mScalar; }


        constexpr iterator       begin()       { return &mScalar; }
        constexpr iterator       end()         { return (&mScalar) + N; }
        constexpr const_iterator begin() const { return &mScalar; }
        constexpr const_iterator end()   const { return (&mScalar) + N; }

        constexpr reverse_iterator       rbegin()       { return reverse_iterator(end()); }
        constexpr reverse_iterator       rend()         { return reverse_iterator(begin()); }
        constexpr const_reverse_iterator rbegin() const { return reverse_iterator(end()); }
        constexpr const_reverse_iterator rend()   const { return reverse_iterator(begin()); }

        constexpr const_iterator         cbegin()  const { return &mScalar; }
        constexpr const_iterator         cend()    const { return (&mScalar) + N; }
        constexpr const_reverse_iterator crbegin() const { return reverse_iterator(end()); }
        constexpr const_reverse_iterator crend()   const { return reverse_iterator(begin()); }

        constexpr std::size_t            size()    const { return N; }

        constexpr basic_complex& operator+=(const value_type rhs) {
            mScalar += rhs;
            return *this;
        }
        constexpr basic_complex& operator-=(const value_type rhs) {
            mScalar -= rhs;
            return *this;
        }
        constexpr basic_complex& operator+=(const basic_complex& rhs) {
            for (std::size_t i = 0; i != N; ++i) (*this)[i] += rhs[i];
            return *this;
        }
        constexpr basic_complex& operator-=(const basic_complex& rhs) {
            for (std::size_t i = 0; i != N; ++i) (*this)[i] -= rhs[i];
            return *this;
        }
        constexpr basic_complex& operator*=(const value_type rhs) {
            for (std::size_t i = 0; i != N; ++i) (*this)[i] *= rhs;
            return *this;
        }
        constexpr basic_complex& operator*=(const basic_complex& rhs) {
            auto s = mScalar * rhs.mScalar - mVector.dot(rhs.mVector);
            auto v = rhs.mScalar * mVector + mScalar * rhs.mVector + rhs.mVector.cross(mVector);
            *this = basic_complex(s, v);
            return *this;
        }
        constexpr basic_complex& operator/=(const basic_complex& rhs) {
            auto s = mScalar * rhs.mScalar + mVector.dot(rhs.mVector);
            auto v = rhs.mScalar * mVector - mScalar * rhs.mVector + mVector.cross(rhs.mVector);
            *this = rsqrt(std::transform_reduce(rhs.begin(), rhs.end(), Ty(0), add, square)) * basic_complex(s, v);
            return *this;
        }
        constexpr basic_complex& operator/=(const value_type rhs) {
            for (std::size_t i = 0; i != N; ++i) (*this)[i] /= rhs;
            return *this;
        }

        constexpr const value_type dot(const basic_complex& rhs) const {
            return std::transform_reduce(begin(), end(), rhs.begin(), Ty(0), add, mul);
        }
        constexpr decltype(auto) conjugate() const {
            return basic_complex<Ty, N>(scalar(), vector());
        }
        constexpr decltype(auto) normalize() const {
            auto z2 = std::transform_reduce(begin(), end(), Ty(0), add, square);
            return rsqrt(z2) * (*this);
        }
    protected:
        Ty mScalar; basic_matrix<Ty, 1, N - 1> mVector;
    };

    template <typename Ty> using binaron   = basic_complex<Ty, 2>;
    template <typename Ty> using quaternion = basic_complex<Ty, 4>;


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
    // Complex algorithms.
    template <typename Ty, std::size_t N>
    constexpr decltype(auto) abs(const basic_complex<Ty, N>& a) {
        auto z2 = std::transform_reduce(a.begin(), a.end(), Ty(0), add, square);
        return sqrt(z2);
    }
    template <typename Ty, std::size_t N>
    constexpr decltype(auto) neg(const basic_complex<Ty, N>& z) {
        auto z2 = std::transform_reduce(z.begin(), z.end(), Ty(0), add, square);
        return rsqrt(z2) * z.conjugate();
    }
    template <typename Ty, std::size_t N>
    constexpr decltype(auto) exp(const basic_complex<Ty, N>& z) {
        auto theta = abs(z.vector());
        return exp(z.scalar()) * basic_complex<Ty, N>{ cos(theta), z.vector() * (sin(theta) / theta)};
    }
    template <typename Ty, std::size_t N>
    constexpr decltype(auto) sqrt(const basic_complex<Ty, N>& z) {
    }

    namespace binaron_literals {
        constexpr decltype(auto) operator""if (long double x) {
            return binaron<float>{0.F, static_cast<float>(x)};
        }
        constexpr decltype(auto) operator""if (unsigned long long x) {
            return binaron<float>{0.F, static_cast<float>(x)};
        }
        constexpr decltype(auto) operator""iF(long double x) {
            return binaron<float>{0.F, static_cast<float>(x)};
        }
        constexpr decltype(auto) operator""iF(unsigned long long x) {
            return binaron<float>{0.F, static_cast<float>(x)};
        }
        constexpr decltype(auto) operator""i(long double x) {
            return binaron<double>{0.0, static_cast<double>(x)};
        }
        constexpr decltype(auto) operator""i(unsigned long long x) {
            return binaron<double>{0.0, static_cast<double>(x)};
        }
    }
    namespace quaternion_literals {
        constexpr decltype(auto) operator""if (long double x) {
            return quaternion<float>{0.F, static_cast<float>(x), 0.F, 0.F};
        }
        constexpr decltype(auto) operator""if (unsigned long long x) {
            return quaternion<float>{0.F, static_cast<float>(x), 0.F, 0.F};
        }
        constexpr decltype(auto) operator""iF(long double x) {
            return quaternion<float>{0.F, static_cast<float>(x), 0.F, 0.F};
        }
        constexpr decltype(auto) operator""iF(unsigned long long x) {
            return quaternion<float>{0.F, static_cast<float>(x), 0.F, 0.F};
        }
        constexpr decltype(auto) operator""i(long double x) {
            return quaternion<double>{0.0, static_cast<double>(x), 0.0, 0.0};
        }
        constexpr decltype(auto) operator""i(unsigned long long x) {
            return quaternion<double>{0.0, static_cast<double>(x), 0.0, 0.0};
        }

        constexpr decltype(auto) operator""jf(long double x) {
            return quaternion<float>{0.F, 0.F, static_cast<float>(x), 0.F};
        }
        constexpr decltype(auto) operator""jf(unsigned long long x) {
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