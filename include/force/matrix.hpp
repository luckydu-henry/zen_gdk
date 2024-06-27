///
/// @file      matrix_view.hpp
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

#include "force/matrix_view.hpp"
#include "force/primary.hpp"
#include "vector.hpp"

namespace force {


    template <typename Ty, std::size_t M, std::size_t N>
    class matrix {
    public:
        using value_type              = Ty;
        using reference               = Ty&;
        using const_reference         = const Ty&;
        using pointer                 = Ty*;
        using const_pointer           = const Ty*;

        using iterator                = pointer;
        using const_iterator          = const_pointer;
        using reverse_iterator        = std::reverse_iterator<iterator>;
        using const_reverse_iterator  = std::reverse_iterator<const_iterator>;

        // Point type is for locate.
        using point_type         = const vector_view<std::ptrdiff_t>;
        using row_coord          = point_type::value_type;
        using col_coord          = point_type::value_type;

        // Matrix view does not support linear iteration.
        using row_view           = vector_view<value_type>;
        using const_row_view     = const vector_view<const value_type>;
        using col_view           = vector_view<value_type>;
        using const_col_view     = const vector_view<const value_type>;
        using row_iterator       = vector_iterator<value_type>;
        using const_row_iterator = const vector_iterator<const value_type>;
        using col_iterator       = vector_iterator<value_type>;
        using const_col_iterator = const vector_iterator<const value_type>;

        static constexpr std::size_t num_elements = M * N;

        constexpr matrix() = default;
        template <typename ... Args> requires std::is_convertible_v<std::common_type_t<Args...>, Ty>
        constexpr matrix(const Args& ... args) : mData{ static_cast<value_type>(args) ..., }{}
        constexpr matrix(const matrix&) = default;
        constexpr matrix(matrix&&)      = default;
        constexpr matrix& operator=(const matrix&) = default;
        constexpr matrix& operator=(matrix&&)      = default;
        constexpr matrix(const matrix_view<value_type> view) { copy_view(view, mData); }
        // 1xN matrix or Nx1 matrix particular.
        constexpr matrix(const vector<value_type, M * N>& vec) { std::ranges::copy(vec, mData); }

        constexpr std::size_t     width()      const { return N; }
        constexpr std::size_t     height()     const { return M; }

        constexpr const_reference             operator[](std::ptrdiff_t i)   const { return mData[i]; }
        constexpr reference                   operator[](std::ptrdiff_t i)         { return mData[i]; }
        constexpr reference                   operator[](const point_type p)       { return mData[p[1] * N + p[0]]; }
        constexpr const_reference             operator[](const point_type p) const { return mData[p[1] * N + p[0]]; }
        constexpr const_reference             front()                        const { return mData[0]; }
        constexpr reference                   front()                              { return mData[0]; }
        constexpr const_reference             back()                         const { return mData[num_elements]; }
        constexpr reference                   back()                               { return mData[num_elements]; }
        constexpr const_pointer               data()                         const { return mData; }
        constexpr pointer                     data()                               { return mData; }
        constexpr std::size_t                 size()                         const { return num_elements; }

        // Get one row or a column.
        constexpr row_view         row_at(row_coord i)       { return row_view      (mData, i * N, N); }
        constexpr const_row_view   row_at(row_coord i) const { return const_row_view(mData, i * N, N); }
        constexpr col_view         col_at(col_coord i)       { return col_view      (mData, i, M, N); }
        constexpr const_col_view   col_at(col_coord i) const { return const_col_view(mData, i, M, N); }

        constexpr row_iterator     row_begin()               { return row_iterator(mData, N); }
        constexpr row_iterator     row_end  ()               { return row_begin() + M; }
        constexpr col_iterator     col_begin(row_iterator i) { return col_iterator(&i[0], 1); }
        constexpr col_iterator     col_end  (row_iterator i) { return col_begin(i) + N; }
        constexpr col_iterator     col_begin()               { return col_iterator(mData, 1); }
        constexpr col_iterator     col_end  ()               { return col_begin() + N; }
        constexpr row_iterator     row_begin(col_iterator i) { return row_iterator(&i[0], N); }
        constexpr row_iterator     row_end  (col_iterator i) { return row_begin(i) + M; }

        constexpr const_row_iterator     row_begin()                     const { return const_row_iterator(mData, N); }
        constexpr const_row_iterator     row_end  ()                     const { return row_begin() + M; }
        constexpr const_col_iterator     col_begin(const_row_iterator i) const { return const_col_iterator(&i[0], 1); }
        constexpr const_col_iterator     col_end  (const_row_iterator i) const { return col_begin(i) + N; }
        constexpr const_col_iterator     col_begin()                     const { return const_col_iterator(mData, 1); }
        constexpr const_col_iterator     col_end  ()                     const { return col_begin() + N; }
        constexpr const_row_iterator     row_begin(const_col_iterator i) const { return const_row_iterator(&i[0], N); }
        constexpr const_row_iterator     row_end  (const_col_iterator i) const { return row_begin(i) + M; }

        constexpr iterator                    begin()         { return iterator(mData); }
        constexpr iterator                    end()           { return begin() + M * N; }
        constexpr const_iterator              begin()   const { return const_iterator(mData); }
        constexpr const_iterator              end()     const { return begin() + num_elements; }
        constexpr const_iterator              cbegin()  const { return begin(); }
        constexpr const_iterator              cend()    const { return end(); }
        constexpr reverse_iterator            rbegin()        { return reverse_iterator(end()); }
        constexpr reverse_iterator            rend()          { return reverse_iterator(begin()); }
        constexpr const_reverse_iterator      rbegin()  const { return const_reverse_iterator(end()); }
        constexpr const_reverse_iterator      rend()    const { return const_reverse_iterator(begin()); }
        constexpr const_reverse_iterator      crbegin() const { return rbegin(); }
        constexpr const_reverse_iterator      crend()   const { return rend(); }

        // Arithmetic operators.
        constexpr matrix  operator+(const matrix& v) const {
            matrix result; for (std::size_t i = 0; i < num_elements; ++i) { result.mData[i] = mData[i] + v.mData[i]; }
            return result;
        }
        constexpr matrix  operator-(const matrix& v) const {
            matrix result; for (std::size_t i = 0; i < num_elements; ++i) { result.mData[i] = mData[i] - v.mData[i]; }
            return result;
        }
        constexpr matrix  operator*(const value_type v) const {
            matrix result; for (std::size_t i = 0; i < num_elements; ++i) { result.mData[i] = mData[i] * v; }
            return result;
        }
        constexpr matrix  operator/(const value_type v) const {
            matrix result; for (std::size_t i = 0; i < num_elements; ++i) { result.mData[i] = mData[i] / v; }
            return result;
        }
        // Special operator multiply for matrix
        template <std::size_t O>
        constexpr matrix<Ty, M, O> operator*(const matrix<Ty, N, O>& mat) const {
            matrix<Ty, M, O> result;
            for (std::ptrdiff_t i = 0; i != M; ++i) {
                for (std::ptrdiff_t j = 0; j != O; ++j) {
                    vector_iterator<Ty> biter(const_cast<Ty*>(mat.data()) + j, O);
                    result[i * O + j] = std::transform_reduce(mData + i * N, mData + (i * N + N),
                        biter, Ty(0), add, mul);
                }
            }
            return result;
        }
        // Arithmetic modifiers.

        // Integer particular (floating point doesn't support)
        constexpr matrix& operator+=(const matrix& p) {
            for (std::size_t i = 0; i < num_elements; ++i) { mData[i] += p.mData[i]; }
            return *this;
        }
        constexpr matrix& operator-=(const matrix& p) {
            for (std::size_t i = 0; i < num_elements; ++i) { mData[i] -= p.mData[i]; }
            return *this;
        }
        constexpr matrix& operator*=(const value_type v) {
            for (std::size_t i = 0; i < num_elements; ++i) { mData[i] *= v; }
            return *this;
        }
        constexpr matrix& operator/=(const value_type v) {
            for (std::size_t i = 0; i < num_elements; ++i) { mData[i] /= v; }
            return *this;
        }
        template <typename ... U> requires std::is_nothrow_convertible_v<std::common_type_t<U...>, value_type> &&
            (sizeof ... (U) == num_elements)
            constexpr matrix& emplace(const U ... args) {
            std::ranges::copy({ args ..., }, mData);
            return *this;
        }
        constexpr operator matrix_view<value_type>() const {
            return matrix_view<value_type>(mData, 0, 0, N, M, N);
        }
        constexpr decltype(auto ) view(const std::ptrdiff_t x, const std::ptrdiff_t y, const std::size_t w, const std::size_t h) const {
            return matrix_view<value_type>(mData, x, y, w, h, N, 1);
        }
        constexpr decltype(auto)  view() const {
            return operator matrix_view<value_type>();
        }

        ~matrix() = default;
    private:
        Ty mData[M * N];
    };

    template <typename Ty, std::size_t M, std::size_t N>
    constexpr decltype(auto) transpose(const matrix<Ty, M, N>& mat) {
        matrix<Ty, N, M> result;
        for (std::ptrdiff_t j = 0; j != M; ++j) {
            for (std::ptrdiff_t i = 0; i != N; ++i) {
                result[i * M + j] = mat[j * N + i];
            }
        }
        return result;
    }
    template <std::size_t N, typename Ty>
    constexpr decltype(auto) id(const Ty& a) {
        matrix<Ty, N, N> result;
        std::fill_n(result.data(), N * N, Ty());
        for (std::ptrdiff_t i = 0; i != N; ++i) { result[i * N + i] = a; }
        return result;
    }
    template <typename Ty, std::size_t M, std::size_t N>
    constexpr decltype(auto) operator*(const matrix<Ty, M, N>& mat, const vector<Ty, N>& vec) {
        vector<Ty, N> result;
        std::ranges::copy(mat * matrix<Ty, N, 1>(vec), result.begin());
        return result;
    }
    template <typename Ty, std::size_t M, std::size_t N>
    constexpr decltype(auto) operator*(const vector<Ty, M>& vec, const matrix<Ty, M, N>& mat) {
        vector<Ty, M> result;
        std::ranges::copy(matrix<Ty, 1, M>(vec) * mat, result.begin());
        return result;
    }
    template <typename Ty, std::size_t M, std::size_t N>
    constexpr decltype(auto) operator*(const Ty& a, const matrix<Ty, M, N>& m) {
        return  m * a;
    }
    template <typename Ty, std::size_t M, std::size_t N>
    constexpr decltype(auto) operator+(const matrix<Ty, M, N>& m) {
        return m;
    }
    template <typename Ty, std::size_t M, std::size_t N>
    constexpr decltype(auto) operator-(const matrix<Ty, M, N>& m) {
        matrix<Ty, M, N> result(m);
        std::ranges::for_each(result, [](auto& k) {k = neg(k); });
        return result;
    }
    template <typename Ty, std::size_t M>
    constexpr decltype(auto) decompose_lu(const matrix<Ty, M, M>& mat) {
        using point = vector<std::ptrdiff_t, 2>;
        matrix<Ty, M, M> lower, upper;
        for (std::ptrdiff_t i = 0; i < M; i++) {
            // Upper Triangular
            for (std::ptrdiff_t k = i; k < M; k++) {
                // Summation of L(i, j) * U(j, k)
                Ty sum = 0;
                for (std::ptrdiff_t j = 0; j < i; j++)
                    sum += (lower[point(i,j)] * upper[point(j,k)]);
                // Evaluating U(i, k)
                upper[point(i,k)] = mat[point(i,k)] - sum;
            }
            // Lower Triangular
            for (std::ptrdiff_t k = i; k < M; k++) {
                if (i == k) {
                    lower[point(i, i)] = Ty(1); // Diagonal as 1
                }
                else {
                    // Summation of L(k, j) * U(j, i)
                    Ty sum = 0;
                    for (std::ptrdiff_t j = 0; j < i; j++)
                        sum += (lower[point(k, j)] * upper[point(j, i)]);
                    // Evaluating L(k, i)
                    lower[point(k,i)] = (mat[point(k,i)] - sum) / upper[point(i,i)];
                }
            }
        }
        return std::make_pair(lower, upper);
    }
    template <typename Ty, std::size_t M>
    constexpr decltype(auto) det(const matrix<Ty, M, M>& mat) {
        auto [_, u] = decompose_lu(mat);
        vector_iterator<Ty> iter(u.data(), 5);
        return std::reduce(iter, iter + M, Ty(1), mul);
    }
    template <typename Ty, std::size_t M>
    constexpr decltype(auto) inv(const matrix<Ty, M, M>& mat) {
        using point = vector<std::ptrdiff_t, 2>;
        auto [lower, upper] = decompose_lu(mat);
        matrix<Ty, M, M> lower_inv, upper_inv;
        // lower inverse
        for (std::ptrdiff_t j = 0; j < M; j++) {
            for (std::ptrdiff_t i = j; i < M; i++) {
                if (i == j)     lower_inv[point(i, j)] = Ty(1) / lower[point(i, j)];
                else if (i < j) lower_inv[point(i, j)] = Ty(0);
                else {
                    Ty s = 0;
                    for (std::ptrdiff_t k = j; k < i; k++) { s += lower[point(i,k)] * lower_inv[point(k,j)];}
                    lower_inv[point(i,j)] = -lower_inv[point(j,j)] * s;
                }
            }
        }
        // upper inverse
        for (std::ptrdiff_t j = 0; j < M; j++) {
            for (std::ptrdiff_t i = j; i >= 0; i--) {
                if (i == j)     upper_inv[point(i, j)] = Ty(1) / upper[point(i,j)];
                else if (i > j) upper_inv[point(i, j)] = Ty(0);
                else {
                    Ty s = 0;
                    for (std::ptrdiff_t k = i + 1; k <= j; k++) { s += upper[point(i,k)] * upper_inv[point(k,j)]; }
                    upper_inv[point(i,j)] = -Ty(1) / upper[point(i,i)] * s;
                }
            }
        }
        return upper_inv * lower_inv;
    }
} //!namespace force::math