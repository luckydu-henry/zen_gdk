#pragma once
#include <algorithm>

#include "tensor_iterator.hpp"
#include "fmath/primary.hpp"

namespace force::math {

    enum class access : unsigned char{
        zero_placeholder = 0,
        horizontal = 1 << 0,
        vertical   = 1 << 1,
    };
    constexpr access operator|(const access& a, const access& b) {
        return static_cast<access>(static_cast<unsigned char>(a) | static_cast<unsigned char>(b));
    }
    constexpr access operator&(const access& a, const access& b) {
        return static_cast<access>(static_cast<unsigned char>(a) & static_cast<unsigned char>(b));
    }

    template <typename SubT, typename Ty, std::size_t Order>
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
        // basic_matrix view can only make sure iteration using iterators are correct.
        constexpr difference_type                         size() const {
            std::size_t pro = 1;
            for (auto i = 0; i != Order; ++i) { pro *= mLengths[1 + (i << 1)]; }
            return pro;
        }

        // Transformation methods.
        constexpr decltype(auto) transpose() const {
            auto [xs, xl, ys, yl] = this->length();
            return SubT(this->data(), ys, yl, xs, xl);
        }
        constexpr decltype(auto) reverse(access seq = access::horizontal | access::vertical) const {
            auto ptr = data();
            auto [xs, xl, ys, yl] = length();
            if (static_cast<bool>(seq & access::horizontal)) { ptr += (xs * xl - xs); xs = -xs; }
            if (static_cast<bool>(seq & access::vertical)) { ptr += (ys * yl - ys); ys = -ys; }
            return SubT(ptr, xs, xl, ys, yl);
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
    class tensor_view <Ty, 1> : public base_tensor_view<tensor_view<Ty, 1>, Ty, 1> {
    public:
        using this_type = tensor_view;
        using base_type = base_tensor_view<tensor_view<Ty, 1>, Ty, 1>;
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

        constexpr this_type view(difference_type off, difference_type s, difference_type l) const {
            return this_type(mPtr + off, s, l);
        }
        constexpr pointer   copy(iterator beg, iterator end, pointer dest) {
            return static_cast<pointer>(std::copy_n(beg, end - beg, dest));
        }
        constexpr pointer   move(iterator beg, iterator end, pointer dest) {
            return static_cast<pointer>(std::move(beg, beg + (end - beg), dest));
        }
        template <typename UnaryFn>
        constexpr decltype(auto) apply(UnaryFn f) {
            for (difference_type i = 0; i < mLengths[0] * mLengths[1]; i += mLengths[0]) {
                mPtr[i] = std::invoke(f, mPtr[i]);
            }
            return *this;
        }
        template <typename BinaryFn, std::ranges::contiguous_range Rng>
        constexpr decltype(auto) apply(const Rng& rg, BinaryFn f) {
            auto p = rg.begin();
            for (difference_type i = 0; i < mLengths[0] * mLengths[1]; i += mLengths[0]) {
                mPtr[i] = std::invoke(f, mPtr[i], *p++);
            }
            return *this;
        }
        template <std::ranges::contiguous_range Rng>
        constexpr decltype(auto) emplace(const Rng& rg) {
            return std::copy_n(rg.begin(), this->size(), begin());
        }
        template <typename ...Vars> requires (std::is_nothrow_convertible_v<std::common_type_t<Vars...>, Ty>)
        constexpr decltype(auto) emplace(const Vars& ... args) {
            using range_type = std::array<std::common_type_t<Vars...>, sizeof ... (Vars)>;
            range_type cache{ args..., };
            return emplace<range_type>(cache);
        }
    protected:
        using base_type::mPtr;
        using base_type::mLengths;
    };

    template <typename Ty>
    class tensor_view<Ty, 2> : public base_tensor_view<tensor_view<Ty, 2>, Ty, 2> {
    public:
        using this_type = tensor_view;
        using base_type = base_tensor_view<tensor_view<Ty, 2>, Ty, 2>;
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

        // basic_matrix can only use vertical and horizontal to access.
        constexpr iterator begin(access seq = access::vertical) {
            switch (seq) {
            case access::horizontal: return iterator(mPtr, mLengths[0], mLengths[2], mLengths[3]);
            case access::vertical: return iterator(mPtr, mLengths[2], mLengths[0], mLengths[1]);
            default: break;
            }
        }
        constexpr iterator end(access seq = access::vertical) {
            switch (seq) {
            case access::horizontal: return begin(seq) + mLengths[1];
            case access::vertical: return begin(seq) + mLengths[3];
            default: break;
            }
        }
        constexpr const_iterator begin(access seq = access::vertical) const {
            switch (seq) {
            case access::horizontal: return iterator(mPtr, mLengths[0], mLengths[2], mLengths[3]);
            case access::vertical: return iterator(mPtr, mLengths[2], mLengths[0], mLengths[1]);
            default: break;
            }
        }
        constexpr const_iterator end(access seq = access::vertical) const {
            switch (seq) {
            case access::horizontal: return begin(seq) + mLengths[1];
            case access::vertical: return begin(seq) + mLengths[3];
            default: break;
            }
        }
        // View provides multiple ways to access raw data.
        // Suits developers favors.
        constexpr reverse_iterator rbegin (access seq = access::vertical) { return reverse_iterator(end(seq)); }
        constexpr reverse_iterator rend   (access seq = access::vertical) { return reverse_iterator(begin(seq)); }
        constexpr reverse_iterator rbegin (access seq = access::vertical)   const { return reverse_iterator(end(seq)); }
        constexpr reverse_iterator rend   (access seq = access::vertical)   const { return reverse_iterator(begin(seq)); }
        constexpr const_iterator   cbegin (access seq = access::vertical)   const { return begin(seq); }
        constexpr const_iterator   cend   (access seq = access::vertical)   const { return end(seq); }
        constexpr reverse_iterator crbegin(access seq = access::vertical)   const { return rbegin(seq); }
        constexpr reverse_iterator crend  (access seq = access::vertical)   const { return rend(seq); }

        constexpr this_type      view(difference_type x_off, difference_type y_off, difference_type w, difference_type h) const {
            return this_type(mPtr + (mLengths[0] * x_off + mLengths[2] * y_off), mLengths[0],w, mLengths[2], h);
        }
        // Current only supports one as stride.
        constexpr decltype(auto) view(difference_type x_off, difference_type y_off, difference_type l, access seq = access::vertical) const {
            auto stride = (seq == access::horizontal) ? mLengths[0] : mLengths[2];
            return tensor_view<value_type, 1>(mPtr + mLengths[0] * x_off + mLengths[2] * y_off, stride, l);
        }
        constexpr pointer copy(iterator beg, iterator end, pointer dest) const {
            pointer p = nullptr;
            for (auto i = beg; i != end; ++i) { p  = std::ranges::copy(i, dest); }
            return p;
        }
        constexpr pointer move(iterator beg, iterator end, pointer dest) const {
            pointer p = nullptr;
            for (auto i = beg; i != end; ++i) { p = std::ranges::move(i, dest); }
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
        template <typename BinaryFn, std::ranges::contiguous_range Rng>
        constexpr decltype(auto) apply(const Rng& rg, BinaryFn f) {
            auto p = rg.begin();
            for (auto j = begin(); j != end(); ++j) {
                for (auto i = j.begin(); i != j.end(); ++i) {
                    *i = std::invoke(f, *i, *p++);
                }
            }
            return *this;
        }
        template <std::ranges::contiguous_range Rng>
        constexpr decltype(auto) emplace(const Rng& rg) {
            auto q = begin();
            for (auto k = rg.begin(); k < rg.end(); k += mLengths[1]) {
                std::copy_n(k, mLengths[1], q); ++q;
            }
            return *this;
        }
        template <typename ...Vars> requires (std::is_nothrow_convertible_v<std::common_type_t<Vars...>, Ty>)
            constexpr decltype(auto) emplace(const Vars& ... args) {
            using range_type = std::array<std::common_type_t<Vars...>, sizeof ... (Vars)>;
            range_type cache{ args..., };
            return emplace<range_type>(cache);
        }
    protected:
        using base_type::mPtr;
        using base_type::mLengths;
    };

    template <typename Ty> using vector_view = tensor_view<Ty, 1>;
    template <typename Ty> using matrix_view = tensor_view<Ty, 2>;
}