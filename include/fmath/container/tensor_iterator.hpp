///
/// @file      tensor_iterator.hpp
/// @brief     Order iterator is a iterator for tensor iteration.
/// @author    HenryDu
/// @date      27.04.2024
/// @copyright © HenryDu 2024. All right reserved.
/// @details
///
/// This iterator is both a range and an iterator (for iterator_order higher then 1)
/// Which provided a way to access data in multi-array's way but under linear
/// storage.
///
#pragma once
#include <iterator>
namespace force::math {

    template <typename Ty, std::size_t Order> requires (Order > 0)
    class tensor_iterator {
    public:
        using iterator_concept = std::contiguous_iterator_tag;
        using this_type        = tensor_iterator;
        using value_type       = Ty;
        using pointer          = value_type*;
        using const_pointer    = const value_type*;
        using reference        = value_type&;
        using const_reference  = const value_type&;
        using difference_type  = std::ptrdiff_t;
        // Range aliases define
        using iterator         = tensor_iterator<value_type, Order - 1>;
        using const_iterator   = const tensor_iterator<value_type, Order - 1>;
        using reverse_iterator = std::reverse_iterator<iterator>;
        using const_reverse_iterator = const std::reverse_iterator<iterator>;

        constexpr tensor_iterator()                        noexcept = default;
        constexpr tensor_iterator(const tensor_iterator&)   noexcept = default;
        constexpr tensor_iterator(tensor_iterator&&)        noexcept = default;

        template <typename ... Ranges> requires (sizeof ... (Ranges) == (Order - 1) << 1) && 
                                                std::is_nothrow_convertible_v<std::common_type_t<Ranges...>, difference_type>
        constexpr explicit tensor_iterator(pointer beg, difference_type s1, Ranges ...rgs) :
        mPtr(beg), mFirstStride(s1), mSubRangeInfo{static_cast<difference_type>(rgs) ... ,} {}
        constexpr explicit tensor_iterator(pointer beg, difference_type s1, const difference_type* rgs) :
        mPtr(beg), mFirstStride(s1), mSubRangeInfo{} {
            std::copy_n(rgs, ((Order - 1) << 1), reinterpret_cast<difference_type*>(mSubRangeInfo));
        }

        constexpr this_type& operator=(const this_type&) noexcept = default;
        constexpr this_type& operator=(this_type&&)      noexcept = default;

        constexpr reference  operator*()                              const noexcept { return *mPtr; }
        constexpr pointer    operator->()                             const noexcept { return  mPtr; }

        constexpr this_type& operator++()                              noexcept { mPtr += mFirstStride; return *this; }
        constexpr this_type  operator++(int)                           noexcept { auto tmp = *this; ++*this; return tmp; }
        constexpr this_type& operator--()                              noexcept { mPtr -= mFirstStride; return *this; }
        constexpr this_type  operator--(int)                           noexcept { auto tmp = *this; --*this; return tmp; }

        constexpr this_type& operator+=(const difference_type d)       noexcept { mPtr += d * mFirstStride; return *this; }
        constexpr this_type  operator+(const difference_type d)  const noexcept { return this_type(mPtr + d * mFirstStride, mFirstStride, mSubRangeInfo); }
        constexpr this_type& operator-=(const difference_type d)       noexcept { mPtr -= d * mFirstStride; return *this; }
        constexpr this_type  operator-(const difference_type d)  const noexcept { return this_type(mPtr - d * mFirstStride, mFirstStride, mSubRangeInfo); }

        constexpr reference  operator[](const difference_type d) const noexcept { return mPtr[d]; }

        friend constexpr difference_type operator-(const this_type& lhs, const this_type& rhs) { return (lhs.mPtr - rhs.mPtr) / lhs.mFirstStride; }
        friend constexpr this_type       operator+(const difference_type d, const this_type& rhs) { return rhs + d; }

        constexpr const_iterator   begin() const noexcept {
            if constexpr (Order == 2) return iterator(mPtr, mSubRangeInfo[0]);
            else return iterator(mPtr, mSubRangeInfo[0], reinterpret_cast<const difference_type*>(mSubRangeInfo + 2));
        }
        constexpr const_iterator   end()   const noexcept {
            return begin() + mSubRangeInfo[1];
        }
        constexpr iterator         begin()       noexcept {
            if constexpr (Order == 2) return iterator(mPtr, mSubRangeInfo[0]);
            else  return iterator(mPtr, mSubRangeInfo[0], reinterpret_cast<const difference_type*>(mSubRangeInfo + 2));
        }
        constexpr iterator         end()         noexcept {
            return begin() + mSubRangeInfo[1];
        }
        constexpr iterator         rbegin()       noexcept { return reverse_iterator(end()); }
        constexpr iterator         rend()         noexcept { return reverse_iterator(begin()); }
        constexpr const_iterator   rbegin() const noexcept { return reverse_iterator(end()); }
        constexpr const_iterator   rend()   const noexcept { return reverse_iterator(begin()); }

        constexpr const_iterator   cbegin()  const noexcept { return begin(); }
        constexpr const_iterator   cend()    const noexcept { return end(); }
        constexpr const_iterator   crbegin() const noexcept { return reverse_iterator(end()); }
        constexpr const_iterator   crend()   const noexcept { return reverse_iterator(begin()); }

        constexpr bool       operator==(const this_type& rhs)  const { return mPtr == rhs.mPtr; }
        constexpr bool       operator!=(const this_type& rhs)  const { return mPtr != rhs.mPtr; }
        constexpr bool       operator<=>(const this_type& rhs) const { return mPtr <=> rhs.mPtr; }
    private:
        pointer                           mPtr;
        difference_type                   mFirstStride;
        difference_type                   mSubRangeInfo[(Order - 1) << 1];
    };
    template <typename Ty>
    class tensor_iterator<Ty, 1> {
    public:
        using iterator_concept = std::contiguous_iterator_tag;
        using this_type = tensor_iterator;
        using value_type = Ty;
        using pointer = Ty*;
        using const_pointer = const Ty*;
        using reference = Ty&;
        using const_reference = const Ty&;
        using difference_type = std::ptrdiff_t;

        constexpr tensor_iterator()                                noexcept = default;
        constexpr tensor_iterator(const tensor_iterator&)          noexcept = default;
        constexpr tensor_iterator(tensor_iterator&&)               noexcept = default;
        constexpr tensor_iterator(pointer beg, difference_type s) noexcept : mPtr(beg), mFirstStride(s) {}

        constexpr this_type& operator=(const this_type&) noexcept = default;
        constexpr this_type& operator=(this_type&&)      noexcept = default;

        constexpr reference  operator*()                              const noexcept { return *mPtr; }
        constexpr pointer    operator->()                             const noexcept { return mPtr; }

        constexpr this_type& operator++()                              noexcept { mPtr += mFirstStride; return *this; }
        constexpr this_type  operator++(int)                           noexcept { auto tmp = *this; ++*this; return tmp; }
        constexpr this_type& operator--()                              noexcept { mPtr -= mFirstStride; return *this; }
        constexpr this_type  operator--(int)                           noexcept { auto tmp = *this; --*this; return tmp; }

        constexpr this_type& operator+=(const difference_type d)       noexcept { mPtr += mFirstStride * d; return *this; }
        constexpr this_type  operator+(const difference_type d)  const noexcept { return this_type(mPtr + mFirstStride * d, mFirstStride); }
        constexpr this_type& operator-=(const difference_type d)       noexcept { mPtr -= mFirstStride * d; return *this; }
        constexpr this_type  operator-(const difference_type d)  const noexcept { return this_type(mPtr - mFirstStride * d, mFirstStride); }

        constexpr reference  operator[](const difference_type d) const noexcept { return *(mPtr + d); }

        friend constexpr difference_type operator-(const this_type& lhs, const this_type& rhs) { return (lhs.mPtr - rhs.mPtr) / lhs.mFirstStride; }
        friend constexpr this_type       operator+(const difference_type d, const this_type& rhs) { return rhs + d; }

        constexpr bool       operator==(const this_type& rhs)  const { return mPtr == rhs.mPtr; }
        constexpr bool       operator!=(const this_type& rhs)  const { return mPtr != rhs.mPtr; }
        constexpr bool       operator<=>(const this_type& rhs) const { return mPtr <=> rhs.mPtr; }

    private:
        pointer         mPtr;
        difference_type mFirstStride;
    };
    // Ordered iterator is a iterator but also a range.
    template <typename Ty>
    struct tensor_iterator_traits : std::false_type {};
    template <typename Ty, std::size_t Order>
    struct tensor_iterator_traits<tensor_iterator<Ty, Order>> : std::true_type {};
    template <typename It>
    constexpr bool is_order_iterator = tensor_iterator_traits<It>::value;

    template <typename Ty> using first_order_iterator = tensor_iterator<Ty, 1>;
    template <typename Ty> using second_order_iterator = tensor_iterator<Ty, 2>;
    template <typename Ty> using third_order_iterator = tensor_iterator<Ty, 3>;
}