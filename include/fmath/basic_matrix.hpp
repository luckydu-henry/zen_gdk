///
/// @file      basic_matrix.hpp
/// @brief     Contains the implementation of the matrix class.
/// @details   ~
/// @author    HenryDu
/// @date      16.05.2024
/// @copyright © HenryDu 2024. All right reserved.
///
#pragma once

#include <algorithm>
#include <numeric>
#include <array>

#include "fmath/primary.hpp"
#include "fmath/tensor_iterator.hpp"
#include "fmath/tensor_view.hpp"
namespace force::math {
    // CRTP to reduce the waste of time for implement.
    template <template <typename, std::size_t, std::size_t> class SubT,
                        typename Ty, std::size_t M, std::size_t N>
    class base_matrix {
    public:
        using value_type        = Ty;
        using reference         = Ty&;
        using const_reference   = const Ty&;
        using pointer           = Ty*;
        using const_pointer     = const Ty* const;
        using difference_type   = std::ptrdiff_t;
        using size_type         = std::size_t;

        constexpr base_matrix()                   noexcept = default;
        constexpr base_matrix(const base_matrix&) noexcept = default;
        constexpr base_matrix(base_matrix&&)      noexcept = default;
        template <typename ... Vars>
        requires ((sizeof ... (Vars) == M * N) && std::is_nothrow_convertible_v<std::common_type_t<Vars...>, Ty>)
        constexpr base_matrix(const Vars& ... args) : mData{ static_cast<value_type>(args) ... , } {}

        constexpr base_matrix& operator=(const base_matrix&) noexcept = default;
        constexpr base_matrix& operator=(base_matrix&&)      noexcept = default;

        // Accessor
        constexpr reference            operator[](const difference_type i)       { return mData[i]; }
        constexpr const_reference      operator[](const difference_type i) const { return mData[i]; }
        constexpr pointer              data()                                    { return mData; }
        constexpr const_pointer        data()                              const { return mData; }
        constexpr size_type            size()                              const { return M * N; }
        constexpr decltype(auto)       lengths()                           const { return std::to_array({ M, N }); }
        constexpr reference            front()                                   { return mData[0]; }
        constexpr const_reference      front()                             const { return mData[0]; }
        constexpr reference            back()                                    { return mData[size()-1]; }
        constexpr const_reference      back()                              const { return mData[size()-1]; }

        // Modifiers
        template <typename UnaryFn>
        constexpr decltype(auto) apply(UnaryFn f) {
            std::for_each_n(mData, size(), [f](auto& v) { v = std::invoke(f, v); });
            return *reinterpret_cast<SubT<Ty, M, N>*>(this);
        }
        template <typename BinaryFn, std::ranges::contiguous_range Rng>
        constexpr decltype(auto) apply(const Rng& rg, BinaryFn f) {
            auto p = rg.begin();
            for (difference_type i = 0; i < size(); ++i) { mData[i] = std::invoke(f, mData[i], *p++); }
            return *reinterpret_cast<SubT<Ty, M, N>*>(this);
        }
        template <std::ranges::contiguous_range Rng>
        constexpr decltype(auto) emplace(const Rng& rg) {
            return std::copy_n(rg.begin(), size(), mData);
        }
        template <typename ...Vars> requires (sizeof ... (Vars) <= M * N) && (std::is_nothrow_convertible_v<std::common_type_t<Vars...>, Ty>)
            constexpr decltype(auto) emplace(const Vars& ... args) {
            using range_type = std::array<std::common_type_t<Vars...>, sizeof ... (Vars)>;
            range_type cache{ args..., };
            return emplace<range_type>(cache);
        }
        // Operators (for common matrix)
        constexpr decltype(auto)       operator+(const SubT<Ty, M, N>& other) const {
            SubT<Ty, M, N> result;
            std::transform(mData, mData + size(), other.data(), result.data(), add);
            return result;
        }
        constexpr decltype(auto)       operator-(const SubT<Ty, M, N>& other) const {
            SubT<Ty, M, N> result;
            std::transform(mData, mData + size(), other.data(), result.data(), sub);
            return result;
        }
        constexpr decltype(auto)       operator*(const Ty b) {
            SubT<Ty, M, N> result;
            std::transform(mData, mData + size(), result.data(), [b](auto g) { return g * b; });
            return result;
        }
        constexpr decltype(auto)       transpose() {
            SubT<Ty, N, M> result;
            for (std::size_t j = 0; j != M; ++j) {
                for (std::size_t i = 0; i != N; ++i) {
                    result[i * M + j] = mData[j * N + i];
                }
            }
            return result;
        }
    protected:
        Ty mData[M * N];
    };

    template <typename Ty, std::size_t M, std::size_t N>
    class basic_matrix {};
    // Matrix
    template <typename Ty, std::size_t M, std::size_t N> requires (M != 1 && N != 1)
    class basic_matrix<Ty, M, N> : public base_matrix<basic_matrix, Ty, M, N> {
    public:
        using this_type = basic_matrix;
        using base_type = base_matrix<basic_matrix, Ty, M, N>;
        using typename base_type::value_type;
        using typename base_type::reference      ;
        using typename base_type::const_reference;
        using typename base_type::pointer        ;
        using typename base_type::const_pointer  ;
        using typename base_type::difference_type;
        using typename base_type::size_type       ;
        using view_type = matrix_view<value_type>;

        using iterator = second_order_iterator<value_type>;
        using const_iterator = const iterator;
        using reverse_iterator = std::reverse_iterator<iterator>;
        using const_reverse_iterator = std::reverse_iterator<iterator>;

        using base_type::base_type;
        using base_type::operator=;
        constexpr basic_matrix(const view_type d) {
            auto p = mData;
            for (auto i = d.begin(); i != d.end(); ++i) {
                for (auto j = i.begin(); j != i.end(); ++j) {
                    *p++ = *j;
                }
            }
        }
        template <std::size_t O>
        constexpr decltype(auto) operator*(const basic_matrix<Ty, N, O>& b) {
            basic_matrix<Ty, M, N> result;
            for (std::size_t i = 0; i != M; ++i) {
                for (std::size_t j = 0; j != O; ++j) {
                    first_order_iterator<Ty> biter(const_cast<Ty*>(b.mData) + j, O);
                    result[i * O + j] = std::transform_reduce(mData + i * N, mData + (i * N + N),
                        biter, Ty(0), add, mul);
                }
            }
            return result;
        }
        // For pure data access, raw matrix only supports y first access.
        constexpr iterator       begin() {
            return second_order_iterator<value_type>(const_cast<pointer>(mData), N, 1, N);
        }
        constexpr iterator       end() {
            return begin() + M;
        }
        constexpr const_iterator begin() const {
            return second_order_iterator<value_type>(const_cast<pointer>(mData), N, 1, N);
        }
        constexpr const_iterator end()   const {
            return begin() + M;
        }
        constexpr reverse_iterator       rbegin() { return reverse_iterator(end()); }
        constexpr reverse_iterator       rend() { return reverse_iterator(begin()); }
        constexpr const_reverse_iterator rbegin() const { return reverse_iterator(end()); }
        constexpr const_reverse_iterator rend()   const { return reverse_iterator(begin()); }

        constexpr const_iterator         cbegin()  const { return begin(); }
        constexpr const_iterator         cend()    const { return end(); }
        constexpr const_reverse_iterator crbegin() const { return rbegin(); }
        constexpr const_reverse_iterator crend()   const { return rend(); }

        constexpr view_type view() const {
            return matrix_view<value_type>(const_cast<pointer>(mData), 1, N, N, M);
        }
        constexpr view_type view(size_type off, size_type y, size_type x) const {
            return matrix_view<value_type>(const_cast<pointer>(mData + off), 1, x, N, y);
        }
        constexpr operator  view_type() const { return view(); }
    protected:
        using base_type::mData;

    };
    // Vector
    template <typename Ty, std::size_t M, std::size_t N> requires (M == 1 || N == 1)
    class basic_matrix<Ty, M, N> : public base_matrix<basic_matrix, Ty, M, N> {
    public:
        using this_type = basic_matrix;
        using base_type = base_matrix<basic_matrix, Ty, M, N>;
        using typename base_type::value_type;
        using typename base_type::reference;
        using typename base_type::const_reference;
        using typename base_type::pointer;
        using typename base_type::const_pointer;
        using typename base_type::difference_type;
        using typename base_type::size_type;
        using view_type = vector_view<value_type>;

        using iterator = first_order_iterator<value_type>;
        using const_iterator = const iterator;
        using reverse_iterator = std::reverse_iterator<iterator>;
        using const_reverse_iterator = std::reverse_iterator<iterator>;

        using base_type::base_type;
        using base_type::operator=;
        constexpr basic_matrix(const view_type d) {
            auto p = mData;
            for (auto i = d.begin(); i != d.end(); ++i) { *p++ = *i; }
        }

        template <std::size_t P, std::size_t Q> requires (P == M && Q == N) || (P == N && Q == M)
        constexpr decltype(auto)     operator*(const basic_matrix<Ty, P, Q>& b) const {
            basic_matrix result;
            std::transform(mData, mData + this->size(), b.data(), result.data(), mul);
            return result;
        }
        template <std::size_t P, std::size_t Q> requires (P == M && Q == N) || (P == N && Q == M)
            constexpr decltype(auto) operator/(const basic_matrix<Ty, P, Q>&b) const {
            basic_matrix result;
            std::transform(mData, mData + this->size(), b.data(), result.data(), div);
            return result;
        }
        // Default to dot product.
        template <std::size_t P, std::size_t Q> requires (P == M && Q == N) || (P == N && Q == M)
        constexpr decltype(auto) dot(const basic_matrix<Ty, P, Q>& b) const {
            return std::transform_reduce(mData, mData + M * N,
                b.mData, Ty(0), add, mul);
        }
        template <std::size_t P, std::size_t Q> requires (P == M && Q == N) || (P == N && Q == M)
        constexpr decltype(auto) cross(const basic_matrix<Ty, P, Q>& b) const {
            if constexpr (M * N == 1) {
                // For complex number (binaron)
                return basic_matrix<Ty, P, Q>(0); // normal multiplication.
            }
            else if constexpr (M * N == 2) {
                // When you want to find out the area of a triangle this would be useful.
                return mData[0] * b.mData[1] - mData[1] * b.mData[0];
            }
            else {
                this_type result;
                for (std::size_t i = 0; i != M * N - 1; ++i) {
                    result[i] = mData[i + 1] * b.mData[i + 2] - mData[i + 2] * b.mData[i + 1];
                }
                result[M * N - 1] = mData[0] * b.mData[1] - mData[1] * b.mData[0];
                return result;
            }
        }
        // Vector specific operations
        constexpr decltype(auto) normalize() const {
            auto a2 = std::transform_reduce(mData, mData + M * N, Ty(0), add, square);
            return mData * rsqrt(a2);
        }

        // For pure data access, raw matrix only supports y first access.
        constexpr iterator       begin() {
            return first_order_iterator<value_type>(mData, 1);
        }
        constexpr iterator       end() {
            return begin() + M * N;
        }
        constexpr const_iterator begin() const {
         return first_order_iterator<value_type>(mData, 1);
        }
        constexpr const_iterator end()   const {
            return begin() + M * N;
        }
        constexpr reverse_iterator       rbegin() { return reverse_iterator(end()); }
        constexpr reverse_iterator       rend() { return reverse_iterator(begin()); }
        constexpr const_reverse_iterator rbegin() const { return reverse_iterator(end()); }
        constexpr const_reverse_iterator rend()   const { return reverse_iterator(begin()); }

        constexpr const_iterator         cbegin()  const { return begin(); }
        constexpr const_iterator         cend()    const { return end(); }
        constexpr const_reverse_iterator crbegin() const { return rbegin(); }
        constexpr const_reverse_iterator crend()   const { return rend(); }

        constexpr view_type view() const {
         return vector_view<value_type>(const_cast<pointer>(mData), 1, M * N);
        }
        constexpr view_type view(size_type off, size_type y) const {
            return vector_view<value_type>(const_cast<pointer>(mData + off), 1, y);
        }
        constexpr operator  view_type() const { return view(); }
    protected:
        using base_type::mData;
    };

    template <std::size_t N, typename Ty>
    constexpr decltype(auto) id(const Ty& a) {
        basic_matrix<Ty, N, N> result;
        std::fill_n(result.data(), N * N, Ty());
        for (std::size_t i = 0; i != N; ++i) { result[i * N + i] = a; }
        return result;
    }
    template <typename Ty, std::size_t M, std::size_t N> requires (M == 1 || N == 1)
    constexpr decltype(auto) abs(const basic_matrix<Ty, M, N>& a) {
        auto a2 = std::transform_reduce(a.data(), a.data() + M * N, Ty(0), add, square);
        return sqrt(a2);
    }
    template <typename Ty, std::size_t M, std::size_t N>
    constexpr decltype(auto) operator*(const basic_matrix<Ty, M, N>& m, const Ty& a) {
        basic_matrix<Ty, M, N> result(m);
        std::ranges::for_each(result, [a](auto& k) {k = k * a; });
        return result;
    }
    template <typename Ty, std::size_t M, std::size_t N>
    constexpr decltype(auto) operator*(const Ty& a, const basic_matrix<Ty, M, N>& m) {
        return operator*(m, a);
    }
    template <typename Ty, std::size_t M, std::size_t N>
    constexpr decltype(auto) operator/(const basic_matrix<Ty, M, N>& m, const Ty& a) {
        basic_matrix<Ty, M, N> result(m);
        std::ranges::for_each(result, [a](auto& k) {k = k / a; });
        return result;
    }
    template <typename Ty, std::size_t M, std::size_t N>
    constexpr decltype(auto) operator+(const basic_matrix<Ty, M, N>& m) {
        return m;
    }
    template <typename Ty, std::size_t M, std::size_t N>
    constexpr decltype(auto) operator-(const basic_matrix<Ty, M, N>& m) {
        basic_matrix<Ty, M, N> result(m);
        std::ranges::for_each(result, [](auto& k) {k = neg(k); });
        return result;
    }
    template <typename Ty, std::size_t M>
    constexpr decltype(auto) decompose_lu(const basic_matrix<Ty, M, M>& mat) {
        basic_matrix<Ty, M, M> lower;
        basic_matrix<Ty, M, M> upper;
#define A(_i, _j) ((_i) * M + (_j))
        for (std::ptrdiff_t i = 0; i < M; i++) {
            // Upper Triangular
            for (std::ptrdiff_t k = i; k < M; k++) {
                // Summation of L(i, j) * U(j, k)
                Ty sum = 0;
                for (std::ptrdiff_t j = 0; j < i; j++)
                    sum += (lower[A(i,j)] * upper[A(j,k)]);
                // Evaluating U(i, k)
                upper[A(i,k)] = mat[A(i,k)] - sum;
            }
            // Lower Triangular
            for (std::ptrdiff_t k = i; k < M; k++) {
                if (i == k) {
                    lower[A(i, i)] = Ty(1); // Diagonal as 1
                }
                else {
                    // Summation of L(k, j) * U(j, i)
                    Ty sum = 0;
                    for (std::ptrdiff_t j = 0; j < i; j++)
                        sum += (lower[A(k, j)] * upper[A(j, i)]);
                    // Evaluating L(k, i)
                    lower[A(k,i)]
                        = (mat[A(k,i)] - sum) / upper[A(i,i)];
                }
            }
        }
        return std::make_pair(lower, upper);
    }
    template <typename Ty, std::size_t M>
    constexpr decltype(auto) det(const basic_matrix<Ty, M, M>& mat) {
        auto [_, u] = decompose_lu(mat);
        first_order_iterator<float> iter(u.data(), 5);
        return std::reduce(iter, iter + M, Ty(1), mul);
    }
    template <typename Ty, std::size_t M>
    constexpr decltype(auto) inv(const basic_matrix<Ty, M, M>& mat) {
        auto [lower, upper] = decompose_lu(mat);
        basic_matrix<Ty, M, M> lower_inv;
        basic_matrix<Ty, M, M> upper_inv;
        // lower inverse
        for (std::ptrdiff_t j = 0; j < M; j++) {
            for (std::ptrdiff_t i = j; i < M; i++) {
                if (i == j) lower_inv[A(i, j)] = Ty(1) / lower[A(i, j)];
                else if (i < j) lower_inv[A(i,j)] = Ty(0);
                else {
                    Ty s = 0;
                    for (std::ptrdiff_t k = j; k < i; k++) {
                        s += lower[A(i,k)] * lower_inv[A(k,j)];
                    }
                    lower_inv[A(i,j)] = -lower_inv[A(j,j)] * s;
                }
            }
        }
        // upper inverse
        for (std::ptrdiff_t j = 0; j < M; j++) {
            for (std::ptrdiff_t i = j; i >= 0; i--) {
                if (i == j) upper_inv[A(i,j)] = Ty(1) / upper[A(i,j)];
                else if (i > j) upper_inv[A(i, j)] = Ty(0);
                else {
                    Ty s = 0;
                    for (std::ptrdiff_t k = i + 1; k <= j; k++) {
                        s += upper[A(i,k)] * upper_inv[A(k,j)];
                    }
                    upper_inv[A(i,j)] = -Ty(1) / upper[A(i,i)] * s;
                }
            }
        }
        return upper_inv * lower_inv;
    }
#undef A
} //!namespace force::math