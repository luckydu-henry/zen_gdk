///
/// \file      vector.hpp
/// \brief     Arithmetic vector container.
/// \details   ~
/// \author    HenryDu
/// \date      6.06.2024
/// \copyright © HenryDu 2024. All right reserved.
///
#pragma once

#include <concepts>
#include <iterator>
#include <numeric>

#include "vector.hpp"
#include "force/vector_view.hpp"
#include "force/primary.hpp"
namespace force {
    template <typename Ty, std::size_t N>
    class vector {
    public:
        using value_type       = Ty;
        using pointer          = value_type*;
        using reference        = value_type&;
        using const_pointer    = const value_type*;
        using const_reference  = const value_type&;

        using iterator               = vector_iterator<value_type>;
        using const_iterator         = vector_iterator<const value_type>;
        using reverse_iterator       = std::reverse_iterator<iterator>;
        using const_reverse_iterator = std::reverse_iterator<const_iterator>;

        static constexpr std::size_t num_dimensions = N;

        template <typename ... U>
            requires ((sizeof ... (U) == N) && std::is_nothrow_convertible_v<std::common_type_t<U...>, Ty>)
        constexpr vector(const U& ... args) : mData{ static_cast<value_type>(args) ... , } {}
        constexpr vector(const vector&) = default;
        constexpr vector(vector&&)      = default;
        constexpr vector()              = default;
        constexpr vector(const vector_view<value_type> v) noexcept { std::ranges::copy(v, begin()); }


        constexpr vector& operator=(const vector&) = default;
        constexpr vector& operator=(vector&&)      = default;
        constexpr vector& operator=(const vector_view<value_type> v) { std::ranges::copy(v, begin()); return *this; }

        // Arithmetic operators.
        constexpr vector  operator<<(std::ptrdiff_t s) const {
            vector result; for (std::size_t i = 0; i < num_dimensions; ++i) { result.mData[i] = mData[i] << s; }
            return result;
        }
        constexpr vector  operator>>(std::ptrdiff_t s) const {
            vector result; for (std::size_t i = 0; i < num_dimensions; ++i) { result.mData[i] = mData[i] >> s; }
            return result;
        }
        constexpr vector  operator+(const vector& v) const {
            vector result; for (std::size_t i = 0; i < num_dimensions; ++i) { result.mData[i] = mData[i] + v.mData[i]; }
            return result;
        }
        constexpr vector  operator-(const vector& v) const {
            vector result; for (std::size_t i = 0; i < num_dimensions; ++i) { result.mData[i] = mData[i] - v.mData[i]; }
            return result;
        }
        constexpr vector  operator*(const vector& v) const {
            vector result; for (std::size_t i = 0; i < num_dimensions; ++i) { result.mData[i] = mData[i] * v.mData[i]; }
            return result;
        }
        constexpr vector  operator/(const vector& v) const {
            vector result; for (std::size_t i = 0; i < num_dimensions; ++i) { result.mData[i] = mData[i] / v.mData[i]; }
            return result;
        }
        constexpr vector  operator*(const value_type v) const {
            vector result; for (std::size_t i = 0; i < num_dimensions; ++i) { result.mData[i] = mData[i] * v; }
            return result;
        }
        constexpr vector  operator/(const value_type v) const {
            vector result; for (std::size_t i = 0; i < num_dimensions; ++i) { result.mData[i] = mData[i] / v; }
            return result;
        }

        // Two special multiplication method for vector.
        constexpr     value_type   dot(const vector& v) const {
            return std::transform_reduce(mData, mData + num_dimensions, v.mData, value_type(0), add, mul);
        }
        constexpr decltype(auto) cross(const vector& v) const {
            if constexpr      (num_dimensions == 1) {
                // Complex would use this
                return vector<value_type, 1>(0);
            }
            else if constexpr (num_dimensions == 2) {
                // Useful when trying to find the area of a triangle in descartes coordinate.
                return mData[0] * v.mData[1] - mData[1] * v.mData[0];
            }
            // Although only vector in dimension 2^n - 1 has correct definition of cross product,
            // I still provided the common method to calculate.
            else {
                vector result;
                for (std::size_t i = 0; i != N - 1; ++i) {
                    result[i] = mData[i + 1] * v.mData[i + 2] - mData[i + 2] * v.mData[i + 1];
                }
                result[N - 1] = mData[0] * v.mData[1] - mData[1] * v.mData[0];
                return result;
            }
        }

        // Arithmetic modifiers.

        // Integer particular (floating point doesn't support)
        constexpr vector& operator<<=(std::ptrdiff_t s) {
            for (std::size_t i = 0; i < num_dimensions; ++i) { mData[i] <<= s; }
            return *this;
        }
        constexpr vector& operator>>=(std::ptrdiff_t s) {
            for (std::size_t i = 0; i < num_dimensions; ++i) { mData[i] >>= s; }
            return *this;
        }
        constexpr vector& operator+=(const vector& p) {
            for (std::size_t i = 0; i < num_dimensions; ++i) { mData[i] += p.mData[i]; }
            return *this;
        }
        constexpr vector& operator-=(const vector& p) {
            for (std::size_t i = 0; i < num_dimensions; ++i) { mData[i] -= p.mData[i]; }
            return *this;
        }
        constexpr vector& operator*=(const vector& p) {
            for (std::size_t i = 0; i < num_dimensions; ++i) { mData[i] *= p.mData[i]; }
            return *this;
        }
        constexpr vector& operator/=(const vector& p) {
            for (std::size_t i = 0; i < num_dimensions; ++i) { mData[i] /= p.mData[i]; }
            return *this;
        }
        constexpr vector& operator*=(const value_type v) {
            for (std::size_t i = 0; i < num_dimensions; ++i) { mData[i] *= v; }
            return *this;
        }
        constexpr vector& operator/=(const value_type v) {
            for (std::size_t i = 0; i < num_dimensions; ++i) { mData[i] /= v; }
            return *this;
        }
        template <typename ... U> requires std::is_nothrow_convertible_v<std::common_type_t<U...>, value_type> &&
            (sizeof ... (U) == num_dimensions)
        constexpr vector& emplace(const U ... args) {
            std::ranges::copy({ args ..., }, mData);
            return *this;
        }
        // View conversion methods.
        template <std::ptrdiff_t B, std::size_t L>
        constexpr  decltype(auto) subvec() const {
            vector<value_type, L - B> result;
            std::copy_n(mData + B, L, result.data());
            return result;
        }
        constexpr operator vector_view<value_type>() const {
            return vector_view<value_type>(data(), 0, size());
        }
        constexpr decltype(auto) view(const std::ptrdiff_t x, const std::size_t l) const {
            return vector_view<value_type>(data(), x, l);
        }
        constexpr decltype(auto) view() const {
            return operator vector_view<value_type>();
        }

        // Accessors.
        constexpr const_reference             operator[](std::size_t i) const { return mData[i]; }
        constexpr reference                   operator[](std::size_t i)       { return mData[i]; }
        constexpr const_reference             front()                   const { return mData[0]; }
        constexpr reference                   front()                         { return mData[0]; }
        constexpr const_reference             back()                    const { return mData[num_dimensions]; }
        constexpr reference                   back()                          { return mData[num_dimensions]; }
        constexpr const_pointer               data() const                    { return mData; }
        constexpr pointer                     data()                          { return mData; }
        constexpr std::size_t                 size() const                    { return num_dimensions; }

        // Iterators.
        constexpr iterator                    begin()         { return iterator(mData); }
        constexpr iterator                    end()           { return begin() + num_dimensions; }
        constexpr const_iterator              begin()   const { return const_iterator(mData); }
        constexpr const_iterator              end()     const { return begin() + num_dimensions; }
        constexpr const_iterator              cbegin()  const { return begin(); }
        constexpr const_iterator              cend()    const { return end(); }
        constexpr reverse_iterator            rbegin()        { return reverse_iterator(end()); }
        constexpr reverse_iterator            rend()          { return reverse_iterator(begin()); }
        constexpr const_reverse_iterator      rbegin()  const { return const_reverse_iterator(end()); }
        constexpr const_reverse_iterator      rend()    const { return const_reverse_iterator(begin()); }
        constexpr const_reverse_iterator      crbegin() const { return rbegin(); }
        constexpr const_reverse_iterator      crend()   const { return rend(); }

        ~vector() = default;
    private:
        Ty  mData[num_dimensions];
    };
    // Two special operators.
    template <typename Ty, std::size_t N>
    constexpr decltype(auto) operator*(Ty mu, const vector<Ty, N>& v) {
        return v * mu;
    }
    template <typename Ty, std::size_t N>
    constexpr decltype(auto) operator/(Ty mu, const vector<Ty, N>& v) {
        vector<Ty, N> result;
        for (int i = 0; i != N; ++i) {
            result[i] = mu / v[i];
        }
        return result;
    }
    template <typename Ty, std::size_t N>
    constexpr decltype(auto) abs(const vector<Ty, N>& a) {
        auto a2 = std::transform_reduce(a.data(), a.data() + N, Ty(0), add, square);
        return sqrt(a2);
    }
    template <typename Ty, std::size_t N>
    constexpr decltype(auto) normalize(const vector<Ty, N>& a) {
        auto a2 = std::transform_reduce(a.data(), a.data() + N, Ty(0), add, square);
        return rsqrt(a2) * a;
    }
}