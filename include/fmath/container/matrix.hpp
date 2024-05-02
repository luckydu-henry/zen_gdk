#pragma once
#include "fmath/container/tensor_iterator.hpp"
#include "fmath/container/tensor_view.hpp"
namespace force::math {
    template <std::size_t M, std::size_t N>
    constexpr bool is_vector_layout_v = (M == 1 || N == 1);
    template <std::size_t M, std::size_t N>
    constexpr bool is_square_layout_v = (M == N);

    template <typename Ty, std::size_t M, std::size_t N>
    class basic_matrix {
    public:
        static constexpr bool   is_vector = is_vector_layout_v<M, N>;
        static constexpr size_t iterator_order = is_vector ? 1 : 2;

        using this_type       = basic_matrix;
        using value_type      = Ty;
        using view_type       = std::conditional_t<is_vector, vector_view<value_type>, matrix_view<value_type>>;
        using reference       = Ty&;
        using const_reference = const Ty&;
        using pointer         = Ty*;
        using const_pointer   = const value_type* const;
        using size_type       = std::size_t;

        using iterator                = tensor_iterator<value_type, iterator_order>;
        using const_iterator          = const iterator;
        using reverse_iterator        = std::reverse_iterator<iterator>;
        using const_reverse_iterator  = const std::reverse_iterator<iterator>;

        template <typename ... Vars> requires (sizeof ... (Vars) == M * N)
        constexpr basic_matrix(const Vars& ... args) : mData{static_cast<value_type>(args) ... ,} {}
        constexpr basic_matrix(const view_type d) : mData{} {
            std::copy_n(d.data(), d.size(), mData);
        }
        template <std::contiguous_iterator It>
        constexpr basic_matrix(It beg, It end) {
            std::copy(beg, end, mData);
        }
        template <std::ranges::contiguous_range Rng>
        constexpr basic_matrix(const Rng& rg) {
            std::ranges::copy(rg, mData);
        }
        constexpr basic_matrix() noexcept = default;
        constexpr basic_matrix(const basic_matrix&) noexcept = default;
        constexpr basic_matrix(basic_matrix&&)      noexcept = default;
        // For pure data access, raw matrix only supports y first access.
        constexpr iterator       begin() {
            if constexpr (is_vector) { return first_order_iterator<value_type>(mData, 1); }
            else return second_order_iterator<value_type>{mData, N, 1, N};
        }
        constexpr iterator       end() {
            if constexpr (is_vector) { return begin() + M * N; }
            else return begin() + M;
        }
        constexpr const_iterator begin() const {
            if constexpr (is_vector) { return first_order_iterator<value_type>(mData, 1); }
            else return second_order_iterator<value_type>{mData, N, 1, N};
        }
        constexpr const_iterator end()   const {
            if constexpr (is_vector) { return begin() + M * N; }
            else return begin() + M;
        }
        constexpr reverse_iterator       rbegin() { return reverse_iterator(end()); }
        constexpr reverse_iterator       rend()   { return reverse_iterator(begin()); }
        constexpr const_reverse_iterator rbegin() const { return reverse_iterator(end()); }
        constexpr const_reverse_iterator rend()   const { return reverse_iterator(begin()); }

        constexpr const_iterator         cbegin()  const { return begin(); }
        constexpr const_iterator         cend()    const { return end(); }
        constexpr const_reverse_iterator crbegin() const { return rbegin(); }
        constexpr const_reverse_iterator crend()   const { return rend(); }
        // Waiting for C++23 multidimensional operator[] for straight forward access.
        constexpr reference                             operator[](const size_type i)       { 
            return mData[i]; 
        }
        constexpr const_reference                       operator[](const size_type i) const {
            return mData[i];
        }
        constexpr pointer                               data() {
            return mData;
        }
        constexpr const_pointer                         data() const {
            return mData;
        }
        // Length equals to size only when matrix is a vector, otherwise you should use a
        // structure binding to get its return values.
        constexpr decltype(auto)                        length() const {
            if constexpr (is_vector) { return M * N; }
            else return std::to_array({ M, N });
        }
        // size returns size of data array no matter what kind of layout.
        // You might want to use this when you are trying to do 1d iteration.
        constexpr const size_type                       size() const {
            return M * N;
        }
        constexpr view_type view() const {
            if constexpr (is_vector) { return vector_view<value_type>(const_cast<pointer>(mData), 1, M * N); }
            else return matrix_view<value_type>(const_cast<pointer>(mData), 1, N, N, M);
        }
        // This x parameter available only when matrix is not a vector.
        constexpr view_type view(size_type off, size_type y, size_type x = 1) const {
            if constexpr (is_vector) { return vector_view<value_type>(const_cast<pointer>(mData + off), 1, y); }
            else return matrix_view<value_type>(const_cast<pointer>(mData + off), 1, x, N, y);
        }
        ~basic_matrix() = default;
    private:
        value_type mData[M * N];
    };
} //!namespace force::math