#pragma once
#include <algorithm>
#include <numeric>
#include "fmath/container/matrix.hpp"
namespace force::math {
    template <std::size_t N, typename Ty>
    constexpr decltype(auto) id(const Ty& a) {
        basic_matrix<Ty, N, N> result;
        std::fill_n(result.data(), N * N, Ty());
        for (std::size_t i = 0; i != N; ++i) { result[i * N + i] = a; }
        return result;
    }
    template <typename Ty, std::size_t M, std::size_t N>
    constexpr decltype(auto) operator+(const basic_matrix<Ty, M, N>& a, const basic_matrix<Ty, M, N>& b) {
        basic_matrix<Ty, M, N> result;
        std::transform(a.data(), a.data() + M * N, b.data(), result.data(), add);
        return result;
    }
    template <typename Ty, std::size_t M, std::size_t N>
    constexpr decltype(auto) operator+(const basic_matrix<Ty, M, N>& a) {
        return a;
    }
    template <typename Ty, std::size_t M, std::size_t N>
    constexpr decltype(auto) operator-(const basic_matrix<Ty, M, N>& a, const basic_matrix<Ty, M, N>& b) {
        basic_matrix<Ty, M, N> result;
        std::transform(a.data(), a.data() + M * N, b.data(), result.data(), sub);
        return result;
    }
    template <typename Ty, std::size_t M, std::size_t N>
    constexpr decltype(auto) operator-(const basic_matrix<Ty, M, N>& a) {
        basic_matrix<Ty, M, N> result = a;
        std::for_each_n(result.data(), M * N, [](auto& x) {x = -x; });
        return result;
    }
    template <typename Ty, std::size_t M, std::size_t N>
    constexpr decltype(auto) operator*(const basic_matrix<Ty, M, N>& a, const basic_matrix<Ty, M, N>& b) {
        basic_matrix<Ty, M, N> result;
        std::transform(a.data(), a.data() + M * N, b.data(), result.data(), mul);
        return result;
    }
    template <typename Ty, std::size_t M, std::size_t N>
    constexpr decltype(auto) operator*(const basic_matrix<Ty, M, N>& a, const Ty b) {
        basic_matrix<Ty, M, N> result;
        std::transform(a.data(), a.data() + M * N, result.data(), [=](auto g) { return g * b; });
        return result;
    }
    template <typename Ty, std::size_t M, std::size_t N>
    constexpr decltype(auto) operator*(const Ty b, const basic_matrix<Ty, M, N>& a) {
        basic_matrix<Ty, M, N> result;
        std::transform(a.data(), a.data() + M * N, result.data(), [=](auto g) { return g * b; });
        return result;
    }
    template <typename Ty, std::size_t M, std::size_t N>
    constexpr decltype(auto) operator/(const basic_matrix<Ty, M, N>& a, const basic_matrix<Ty, M, N>& b) {
        basic_matrix<Ty, M, N> result;
        std::transform(a.data(), a.data() + M * N, b.data(), result.data(), div);
        return result;
    }
    template <typename Ty, std::size_t M, std::size_t N>
    constexpr decltype(auto) operator/(const basic_matrix<Ty, M, N>& a, const Ty b) {
        basic_matrix<Ty, M, N> result;
        std::transform(a.data(), a.data() + M * N, result.data(), [=](auto g) { return g / b; });
        return result;
    }
    template <typename Ty, std::size_t M, std::size_t N, std::size_t O>
    constexpr decltype(auto) dot_product(const basic_matrix<Ty, M, N>& a, const basic_matrix<Ty, N, O>& b) {
        basic_matrix<Ty, M, O>   result;
        for (std::size_t i = 0; i != M; ++i) {
            for (std::size_t j = 0; j != O; ++j) {
                first_order_iterator<Ty> biter(const_cast<Ty*>(b.data()) + j, O);
                result[i * O + j] = std::transform_reduce(a.data() + i * N, a.data() + (i * N + N),
                    biter, Ty(0), add, mul);
            }
        }
        return result;
    }
    template <typename Ty, std::size_t M, std::size_t N>
    constexpr decltype(auto) transpose(const basic_matrix<Ty, M, N>& mat) {
        basic_matrix<Ty, N, M> result;
        for (std::size_t j = 0; j != M; ++j) {
            for (std::size_t i = 0; i != N; ++i) {
                result[i * M + j] = mat[j * N + i];
            }
        }
        return result;
    }
    template <typename Ty, std::size_t M, std::size_t N> requires is_vector_layout_v<M, N>
    constexpr decltype(auto) length(const basic_matrix<Ty, M, N>& a) {
        auto a2 = std::transform_reduce(a.data(), a.data() + M * N, Ty(0), add, square);
        return sqrt(a2);
    }
    template <typename Ty, std::size_t M, std::size_t N> requires is_vector_layout_v<M, N>
    constexpr decltype(auto) norm(const basic_matrix<Ty, M, N>& a) {
        auto a2 = std::transform_reduce(a.data(), a.data() + M * N, Ty(0), add, square);
        return rsqrt(a2) * a;
    }

    // Below can be used to do image process
    // These following operators are for views.
    // Views operation won't change the view itself neither memory it points to.
    template <typename Ty>
    constexpr decltype(auto) transpose(const matrix_view<Ty> mat) {
        auto [xs, xl, ys, yl] = mat.length();
        return matrix_view<Ty>(mat.data(), ys, yl, xs, xl);
    }
    template <typename Ty>
    constexpr decltype(auto) reverse(const matrix_view<Ty> mat, access_sequence seq = sequences::seq_x | sequences::seq_y) {
        auto ptr = mat.data();
        auto [xs, xl, ys, yl] = mat.length();
        if (static_cast<bool>(seq & sequences::seq_x)) { ptr += (xs * xl - xs); xs = -xs; }
        if (static_cast<bool>(seq & sequences::seq_y)) { ptr += (ys * yl - ys); ys = -ys; }
        return matrix_view<Ty>(ptr, xs, xl, ys, yl);
    }
}