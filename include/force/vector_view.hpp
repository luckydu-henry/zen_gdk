///
/// \file      vector_view.hpp
/// \brief     Vector view provides a way to access memory in any delta.
/// \details   It could be pretty useful when dealing with cases like multidimensional arrays
///            or if you simply just want a sequence memory pack in a particular format.
/// \author    HenryDu
/// \date      7.06.2024
/// \copyright © HenryDu 2024. All right reserved.
///
#pragma once

#include <iterator>
#include <ranges>

namespace force {
    ///
    /// \class   vector_iterator
    /// \brief   This iterator can move its pointer in any delta (can be negative)
    /// \details ~
    /// \tparam  Ty - Value type.
    ///
    template <typename Ty>
    class vector_iterator {
    public:
        using iterator_concept = std::contiguous_iterator_tag;
        using this_type = vector_iterator;
        using value_type = Ty;
        using pointer = Ty*;
        using const_pointer = const Ty*;
        using reference = Ty&;
        using const_reference = const Ty&;
        using difference_type = std::ptrdiff_t;

        constexpr vector_iterator()                                noexcept = default;
        constexpr vector_iterator(const vector_iterator&)    noexcept = default;
        constexpr vector_iterator(vector_iterator&&)         noexcept = default;
        constexpr vector_iterator(const_pointer beg, difference_type s = 1)  noexcept : mPtr(const_cast<pointer>(beg)), mDelta(s) {}

        constexpr this_type& operator=(const this_type&) noexcept = default;
        constexpr this_type& operator=(this_type&&)      noexcept = default;

        constexpr reference  operator*()                              const noexcept { return *mPtr; }
        constexpr pointer    operator->()                             const noexcept { return mPtr; }

        constexpr this_type& operator++()                              noexcept { mPtr += mDelta; return *this; }
        constexpr this_type  operator++(int)                           noexcept { auto tmp = *this; ++*this; return tmp; }
        constexpr this_type& operator--()                              noexcept { mPtr -= mDelta; return *this; }
        constexpr this_type  operator--(int)                           noexcept { auto tmp = *this; --*this; return tmp; }

        constexpr this_type& operator+=(const difference_type d)       noexcept { mPtr += mDelta * d; return *this; }
        constexpr this_type  operator+(const difference_type d)  const noexcept { return this_type(mPtr + mDelta * d, mDelta); }
        constexpr this_type& operator-=(const difference_type d)       noexcept { mPtr -= mDelta * d; return *this; }
        constexpr this_type  operator-(const difference_type d)  const noexcept { return this_type(mPtr - mDelta * d, mDelta); }

        constexpr reference  operator[](const difference_type d) const noexcept { return *(mPtr + d); }

        friend constexpr difference_type operator-(const this_type& lhs, const this_type& rhs) { return (lhs.mPtr - rhs.mPtr) / lhs.mDelta; }
        friend constexpr this_type       operator+(const difference_type d, const this_type& rhs) { return rhs + d; }

        constexpr bool       operator==(const this_type& rhs)  const { return mPtr == rhs.mPtr; }
        constexpr bool       operator!=(const this_type& rhs)  const { return mPtr != rhs.mPtr; }
        constexpr bool       operator<=>(const this_type& rhs) const { return mPtr <=> rhs.mPtr; }

        constexpr difference_type stride() const { return mDelta; }
        constexpr void            stride(const difference_type d) { mDelta = d; }

        ~vector_iterator() = default;
    private:
        pointer         mPtr;
        difference_type mDelta;
    };
    ///
    /// \class   vector_view
    /// \brief   Range uses to simplify multidimensional loop even more.
    /// \details ~ 
    /// \tparam  Ty - Value type.
    ///
    template <typename Ty>
    class vector_view {
    public:
        using this_type              = vector_view;
        using value_type             = Ty;
        using reference              = value_type&;
        using pointer                = value_type*;
        using const_pointer          = const value_type*;
        using const_reference        = const value_type&;

        using iterator               = vector_iterator<value_type>;
        using const_iterator         = vector_iterator<const value_type>;
        using reverse_iterator       = std::reverse_iterator<iterator>;
        using const_reverse_iterator = std::reverse_iterator<const_iterator>;
        using difference_type        = std::ptrdiff_t;


        constexpr vector_view() noexcept = default;
        constexpr vector_view(const_pointer beg, std::ptrdiff_t x, std::size_t l, std::ptrdiff_t d = 1) noexcept : mPtr(const_cast<pointer>(&beg[x * d])), mDelta(d), mLength(l) {}

        constexpr const_iterator                  begin()   const { return const_iterator(mPtr, mDelta); }
        constexpr const_iterator                  end()     const { return begin() + mLength; }
        constexpr iterator                        begin()         { return iterator(mPtr, mDelta); }
        constexpr iterator                        end()           { return begin() + mLength; }
        constexpr const_iterator                  cbegin()  const { return begin(); }
        constexpr const_iterator                  cend()    const { return end(); }
        constexpr const_reverse_iterator          rbegin()  const { return const_reverse_iterator(end()); }
        constexpr const_reverse_iterator          rend()    const { return const_reverse_iterator(begin()); }
        constexpr const_reverse_iterator          rbegin()        { return reverse_iterator(end()); }
        constexpr const_reverse_iterator          rend()          { return reverse_iterator(begin()); }
        constexpr const_reverse_iterator          crbegin() const { return const_reverse_iterator(cend()); }
        constexpr const_reverse_iterator          crend()   const { return const_reverse_iterator(cbegin()); }

        constexpr std::ptrdiff_t delta() const                            { return mDelta; }
        constexpr void           delta(std::ptrdiff_t d)                  { mDelta = d; }
        constexpr std::size_t    length() const                           { return mLength; }
        constexpr void           length(std::size_t l)                    { mLength = l; }
        constexpr std::size_t    size()   const                           { return mLength; }

        constexpr const_pointer        data()                              const { return mPtr; }
        constexpr pointer              data()                                    { return mPtr; }
        constexpr const_reference      front()                             const { return *begin(); }
        constexpr reference            front()                                   { return *begin(); }
        constexpr const_reference      back()                              const { return *end(); }
        constexpr reference            back()                                    { return *end(); }
        constexpr const_reference      operator[](const difference_type d) const { return mPtr[d * mDelta]; }
        constexpr reference            operator[](const difference_type d)       { return mPtr[d * mDelta]; }

        /// \brief Use these to operate view as a slider window.
        constexpr vector_view view(const std::ptrdiff_t t, const std::size_t l) const {
            return vector_view(mPtr, t, l, mDelta);
        }

        constexpr bool       operator==(const vector_view& other) const { return std::ranges::equal(*this, other); }
        constexpr bool       operator==(const_pointer ptr)              const { return std::equal(begin(), end(), ptr); }

        constexpr vector_view& operator<<=(std::ptrdiff_t s) {
            for (std::size_t i = 0; i < length(); ++i) { mPtr[i] <<= s; } return *this;
        }
        constexpr vector_view& operator>>=(std::ptrdiff_t s) {
            for (std::size_t i = 0; i < length(); ++i) { mPtr[i] >>= s; } return *this;
        }
        constexpr vector_view& operator+=(const vector_view v) {
            for (std::size_t i = 0; i != length(); ++i) { mPtr[i] += v[i]; } return *this;
        }
        constexpr vector_view& operator-=(const vector_view v) {
            for (std::size_t i = 0; i != length(); ++i) { mPtr[i] += v[i]; } return *this;
        }
        constexpr vector_view& operator*=(const vector_view v) {
            for (std::size_t i = 0; i != length(); ++i) { mPtr[i] += v[i]; } return *this;
        }
        constexpr vector_view& operator/=(const vector_view v) {
            for (std::size_t i = 0; i != length(); ++i) { mPtr[i] += v[i]; } return *this;
        }
        constexpr vector_view& operator*=(const value_type v) {
            for (std::size_t i = 0; i != length(); ++i) { mPtr[i] *= v; }    return *this;
        }
        constexpr vector_view& operator/=(const value_type v) {
            for (std::size_t i = 0; i != length(); ++i) { mPtr[i] /= v; }    return *this;
        }
    private:
        pointer        mPtr;
        std::size_t    mLength;
        std::ptrdiff_t mDelta;
    };

    template <typename Ty, typename Fn>
    constexpr decltype(auto) for_each_view(vector_view<Ty> view, Fn f) {
        std::for_each_n(view.begin(), view.size(), f);
    }
    template <typename Ty, typename OutIt, typename Fn>
    constexpr OutIt copy_view(vector_view<Ty> view, OutIt dest, Fn f) {
        for (auto i = view.begin(); i != view.end(); ++i) {
            // All pass in by reference.
            std::invoke(f, dest, *i);
        }
        return dest;
    }
    template <typename Ty, typename OutIt> requires std::is_same_v< std::iter_value_t<OutIt>, Ty> 
    constexpr const Ty* copy_view(vector_view<Ty> view, OutIt dest) {
        return copy_view(view, dest, [](OutIt& it, const Ty& v) { *it++ = v; });
    }
    /// \brief Use this for quick view reverse, unlike rbegin and rend this one reverses original view by creating a new view.
    /// \tparam Ty   - value_type 
    /// \param  view - View that is ready to be reversed.
    /// \retval      - View that is reversed.
    template <typename Ty>
    constexpr decltype(auto) reverse_view(const vector_view<Ty>& view) {
        return vector_view<Ty>(view.data(),view.delta() * (1 - view.length()), view.length(), -view.delta());
    }
    /// \brief  Use this to make vector view from any kind of contiguous iterator.
    template <std::contiguous_iterator It>
    constexpr decltype(auto) make_vector_view(const It beg, std::size_t l, std::ptrdiff_t d = 1) {
        return vector_view<std::iter_value_t<It>>(&beg[0], 0, l, d);
    }
}