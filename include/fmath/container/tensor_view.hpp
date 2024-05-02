#pragma once
#include <algorithm>

#include "tensor_iterator.hpp"
#include "fmath/algorithm/primary.hpp"

namespace force::math {

    typedef std::byte access_sequence;
    namespace sequences {
        constexpr std::byte seq_x{ 1 << 0 };
        constexpr std::byte seq_y{ 1 << 1 };
    }

    template <typename Ty, std::size_t Order>
    class base_tensor_view {
    public:
        // Basic member aliases.
        using this_type = base_tensor_view;
        using value_type = Ty;
        using pointer = value_type*;
        using const_pointer = const value_type*;
        using reference = value_type&;
        using const_reference = const value_type&;
        using difference_type = std::ptrdiff_t;
        // Iterator aliases.
        using iterator = tensor_iterator<value_type, Order>;
        using const_iterator = const iterator;
        using reverse_iterator = std::reverse_iterator<iterator>;
        using const_reverse_iterator = const reverse_iterator;

        constexpr base_tensor_view()                         noexcept = default;
        constexpr base_tensor_view(const base_tensor_view&)  noexcept = default;
        constexpr base_tensor_view(base_tensor_view&&)       noexcept = default;

        template <std::integral ... Types> requires (sizeof ... (Types) == (Order << 1))
        constexpr base_tensor_view(pointer ptr, Types ... lengths)              noexcept : mPtr(ptr), mLengths{ static_cast<difference_type>(lengths)..., } {}
        constexpr base_tensor_view(pointer ptr, const difference_type* lengths) noexcept : mPtr(ptr), mLengths{} {
            std::copy_n(lengths, Order, mLengths);
        }

        constexpr this_type& operator=(const this_type&) noexcept = default;
        constexpr this_type& operator=(this_type&&)      noexcept = default;

        constexpr reference       operator[](const difference_type i) { return mPtr[i]; }
        constexpr const_reference operator[](const difference_type i) const { return mPtr[i]; }

        constexpr pointer data() { return mPtr; }
        constexpr pointer data() const { return mPtr; }

        constexpr std::array<difference_type, Order << 1> length()   const { return std::to_array(mLengths); }
        // Do not use this to do 1d iteration straight forward
        // Matrix view can only make sure iteration using iterators are correct.
        constexpr difference_type                         size() const {
            std::size_t pro = 1;
            for (auto i = 0; i != Order; ++i) { pro *= mLengths[1 + (i << 1)]; }
            return pro;
        }

        ~base_tensor_view() = default;
    protected:
        pointer         mPtr;
        // Stores stride-length pairs or each dimension (iterator_order)
        difference_type mLengths[Order << 1];
    };

    template <typename Ty, std::size_t Order>
    class tensor_view {};

    template <typename Ty>
    class tensor_view <Ty, 1> : public base_tensor_view<Ty, 1> {
    public:
        using this_type = tensor_view;
        using base_type = base_tensor_view<Ty, 1>;
        using value_type = Ty;
        using pointer = value_type*;
        using const_pointer = const value_type*;
        using reference = value_type&;
        using const_reference = const value_type&;
        using difference_type = std::ptrdiff_t;
        // Iterator aliases.
        using iterator = tensor_iterator<value_type, 1>;
        using const_iterator = const iterator;
        using reverse_iterator = std::reverse_iterator<iterator>;
        using const_reverse_iterator = const reverse_iterator;

        using base_type::base_type;
        using base_type::operator[];
        using base_type::operator=;

        constexpr virtual iterator begin()             { return iterator(mPtr, mLengths[0]); }
        constexpr virtual iterator end()               { return begin() + mLengths[1]; }
        constexpr virtual const_iterator begin() const { return iterator(mPtr, mLengths[0]); }
        constexpr virtual const_iterator end()   const { return begin() + mLengths[1]; }

        constexpr virtual reverse_iterator       rbegin()       { return reverse_iterator(end()); }
        constexpr virtual reverse_iterator       rend()         { return reverse_iterator(begin()); }
        constexpr virtual const_reverse_iterator rbegin() const { return reverse_iterator(end()); }
        constexpr virtual const_reverse_iterator rend()   const { return reverse_iterator(begin()); }

        constexpr const_iterator cbegin()          const { return begin(); }
        constexpr const_iterator cend()            const { return end(); }
        constexpr const_reverse_iterator crbegin() const { return rbegin(); }
        constexpr const_reverse_iterator crend()   const { return rend(); }

        constexpr this_type subview(difference_type off, difference_type s, difference_type l) const {
            return this_type(mPtr + off, s, l);
        }
        constexpr pointer   copy(iterator beg, iterator end, pointer dest) {
            return static_cast<pointer>(std::copy_n(beg, end - beg, dest));
        }
        template <typename UnaryFn>
        constexpr decltype(auto) apply(UnaryFn f) {
            for (difference_type i = 0; i < mLengths[0] * mLengths[1]; i += mLengths[0]) {
                mPtr[i] = std::invoke(f, mPtr[i]);
            }
            return *this;
        }
        template <typename UnaryFn, std::ranges::contiguous_range Rng>
        constexpr decltype(auto) apply(const Rng& rg, UnaryFn f) {
            auto p = rg.begin();
            for (difference_type i = 0; i < mLengths[0] * mLengths[1]; i += mLengths[0]) {
                mPtr[i] = std::invoke(f, mPtr[i], *p++);
            }
            return *this;
        }
        template <std::ranges::contiguous_range Rng>
        constexpr decltype(auto) set(const Rng& rg) {
            std::copy_n(rg.begin(), this->size(), begin());
            return *this;
        }
        // Specialization for initializer list.
        constexpr decltype(auto) qset(std::initializer_list<value_type> lst) {
            return set(lst);
        }
    protected:
        using base_type::mPtr;
        using base_type::mLengths;
    };

    template <typename Ty>
    class tensor_view<Ty, 2> : public base_tensor_view<Ty, 2> {
    public:
        using this_type = tensor_view;
        using base_type = base_tensor_view<Ty, 2>;
        using value_type = Ty;
        using pointer = value_type*;
        using const_pointer = const value_type*;
        using reference = value_type&;
        using const_reference = const value_type&;
        using difference_type = std::ptrdiff_t;
        // Iterator aliases.
        using iterator = tensor_iterator<value_type, 2>;
        using const_iterator = const iterator;
        using reverse_iterator = std::reverse_iterator<iterator>;
        using const_reverse_iterator = const reverse_iterator;

        using base_type::base_type;
        using base_type::operator[];
        using base_type::operator=;

        // Matrix can only use seq_y and seq_x to access.
        constexpr iterator begin(access_sequence seq = sequences::seq_y) {
            switch (seq) {
            case sequences::seq_x: return iterator(mPtr, mLengths[0], mLengths[2], mLengths[3]);
            case sequences::seq_y: return iterator(mPtr, mLengths[2], mLengths[0], mLengths[1]);
            default: break;
            }
            throw std::out_of_range("Unsupported sequence!");
        }
        constexpr iterator end(access_sequence seq = sequences::seq_y) {
            switch (seq) {
            case sequences::seq_x: return begin(seq) + mLengths[1];
            case sequences::seq_y: return begin(seq) + mLengths[3];
            default: break;
            }
            throw std::out_of_range("Unsupported sequence!");
        }
        constexpr const_iterator begin(access_sequence seq = sequences::seq_y) const {
            switch (seq) {
            case sequences::seq_x: return iterator(mPtr, mLengths[0], mLengths[2], mLengths[3]);
            case sequences::seq_y: return iterator(mPtr, mLengths[2], mLengths[0], mLengths[1]);
            default: break;
            }
            throw std::out_of_range("Unsupported sequence!");
        }
        constexpr const_iterator end(access_sequence seq = sequences::seq_y) const {
            switch (seq) {
            case sequences::seq_x: return begin(seq) + mLengths[1];
            case sequences::seq_y: return begin(seq) + mLengths[3];
            default: break;
            }
            throw std::out_of_range("Unsupported sequence!");
        }
        // View provides multiple ways to access raw data.
        // Suits developers favors.
        constexpr reverse_iterator rbegin (access_sequence seq = sequences::seq_y) { return reverse_iterator(end(seq)); }
        constexpr reverse_iterator rend   (access_sequence seq = sequences::seq_y) { return reverse_iterator(begin(seq)); }
        constexpr reverse_iterator rbegin (access_sequence seq = sequences::seq_y)   const { return reverse_iterator(end(seq)); }
        constexpr reverse_iterator rend   (access_sequence seq = sequences::seq_y)   const { return reverse_iterator(begin(seq)); }
        constexpr const_iterator   cbegin (access_sequence seq = sequences::seq_y)   const { return begin(seq); }
        constexpr const_iterator   cend   (access_sequence seq = sequences::seq_y)   const { return end(seq); }
        constexpr reverse_iterator crbegin(access_sequence seq = sequences::seq_y)   const { return rbegin(seq); }
        constexpr reverse_iterator crend  (access_sequence seq = sequences::seq_y)   const { return rend(seq); }

        constexpr this_type      subview(difference_type x_off, difference_type y_off, difference_type w, difference_type h) const {
            return this_type(mPtr + (mLengths[0] * x_off + mLengths[2] * y_off), mLengths[0],w, mLengths[2], h);
        }
        // Current only supports one as stride.
        constexpr decltype(auto) subview(difference_type x_off, difference_type y_off, difference_type l, access_sequence seq = sequences::seq_y) const {
            auto stride = (seq == sequences::seq_x) ? mLengths[0] : mLengths[2];
            return tensor_view<value_type, 1>(mPtr + mLengths[0] * x_off + mLengths[2] * y_off, stride, l);
        }
        constexpr pointer copy(iterator beg, iterator end, pointer dest) {
            pointer p = nullptr;
            for (auto i = beg; i != end; ++i) { p  = std::ranges::copy(i, dest); }
            return p;
        }
        template <typename UnaryFn>
        constexpr decltype(auto) apply(UnaryFn f) {
            for (auto j = begin(); j != end(); ++j) {
                for (auto i = j.begin(); i != j.end(); ++i) {
                    *i = std::invoke(f, *i);
                }
            }
            return *this;
        }
        template <typename UnaryFn, std::ranges::contiguous_range Rng>
        constexpr decltype(auto) apply(const Rng& rg, UnaryFn f) {
            auto p = rg.begin();
            for (auto j = begin(); j != end(); ++j) {
                for (auto i = j.begin(); i != j.end(); ++i) {
                    *i = std::invoke(f, *i, *p++);
                }
            }
            return *this;
        }
        template <std::ranges::contiguous_range Rng>
        constexpr decltype(auto) set(const Rng& rg) {
            auto q = begin();
            for (auto k = rg.begin(); k < rg.end(); k += mLengths[1]) {
                std::copy_n(k, mLengths[1], q); ++q;
            }
            return *this;
        }
        // Specialization for initializer list.
        constexpr decltype(auto) qset(std::initializer_list<value_type> lst) {
            return set(lst);
        }
    protected:
        using base_type::mPtr;
        using base_type::mLengths;
    };

    template <typename Ty> using vector_view = tensor_view<Ty, 1>;
    template <typename Ty> using matrix_view = tensor_view<Ty, 2>;
}
