#pragma once
#include <bit>
#include "fmath/container/matrix.hpp"
namespace force::math {

    template <typename Ty, std::size_t N>
    struct complex_mul_div {};

    template <typename Ty>
    struct complex_mul_div<Ty, 1 << 1> {
        static constexpr void mul(const Ty* a, const Ty* b, Ty* c) {
            Ty p[2] = { a[0], a[1] };
            Ty q[2] = { b[0], b[1] };
            c[0] = p[0] * q[0] - p[1] * q[1];
            c[1] = p[0] * q[1] + p[1] * q[0];
        }
        static constexpr void div(const Ty* a, const Ty* b, Ty* c) {
            Ty p[2] = { a[0], a[1] };
            Ty q[2] = { b[0], b[1] };
            auto d = q[0] * q[0] + q[1] * q[1];
            c[0] = (p[0] * q[0] + p[1] * q[1]) / d;
            c[1] = (p[1] * q[0] - p[0] * q[1]) / d;
        }
    };
    template <typename Ty>
    struct complex_mul_div<Ty, 1 << 2> {
        static constexpr void mul(const Ty* a, const Ty* b, Ty* c) {
            Ty p[4] = { a[0],a[1],a[2],a[3] };
            Ty q[4] = { b[0],b[1],b[2],b[3] };
            c[0] = p[0] * q[0] - p[1] * q[1] - p[2] * q[2] - p[3] * q[3];
            c[1] = p[0] * q[1] + p[1] * q[0] + p[2] * q[3] - p[3] * q[2];
            c[2] = p[0] * q[2] + p[2] * q[0] + p[3] * q[1] - p[1] * q[3];
            c[3] = p[0] * q[3] + p[3] * q[0] + p[1] * q[2] - p[2] * q[1];
        }
        static constexpr void div(const Ty* a, const Ty* b, Ty* c) {
            Ty p[4] = { a[0],a[1],a[2],a[3] };
            Ty q[4] = { b[0],b[1],b[2],b[3] };
            auto d = q[0] * q[0] + q[1] * q[1] + q[2] * q[2] + q[3] * q[3];
            c[0] = (p[0] * q[0] + p[1] * q[1] + p[2] * q[2] + p[3] * q[3]) / d;
            c[1] = (p[1] * q[0] - p[0] * q[1] + p[3] * q[2] - p[2] * q[3]) / d;
            c[2] = (p[2] * q[0] - p[0] * q[2] + p[1] * q[3] - p[3] * q[1]) / d;
            c[3] = (p[2] * q[1] - p[1] * q[2] + p[3] * q[0] - p[0] * q[3]) / d;
        }
    };

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
            complex_mul_div<value_type, N>::mul(data(), rhs.data(), data());
            return *this;
        }
        constexpr basic_complex& operator/=(const basic_complex& rhs) {
            complex_mul_div<value_type, N>::div(data(), rhs.data(), data());
            return *this;
        }
        constexpr basic_complex& operator/=(const value_type rhs) {
            for (std::size_t i = 0; i != N; ++i) (*this)[i] /= rhs;
            return *this;
        }
    protected:
        Ty mScalar; basic_matrix<Ty, 1, N - 1> mVector;
    };

    template <typename Ty> using binarion   = basic_complex<Ty, 2>;
    template <typename Ty> using quaternion = basic_complex<Ty, 4>;
}