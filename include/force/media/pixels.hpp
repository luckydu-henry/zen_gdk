///
/// \file      pixels.hpp
/// \brief     This file contains ways to define and use pixel in force::media.
/// \details   ~
/// \author    HenryDu
/// \date      23.06.2024
/// \copyright © HenryDu 2024. All right reserved.
///
#pragma once

#include <type_traits>
#include <concepts>
#include <bit>
#include <bitset>

#include "force/vector.hpp"

namespace force::media {
    template <typename Ty>
    concept interleaved_pixel_concept = std::is_arithmetic_v<typename Ty::value_type> &&
                                        std::is_default_constructible_v<Ty> && requires(Ty v) {
        std::is_same_v<decltype(v.size()), std::size_t> == true;
        std::is_same_v<decltype(v[0]), Ty&> == true;
    };
    // Homogeneous interleaved pixel refers to a type which has an aligned layout.
    template <typename Ty>
    concept homogeneous_interleaved_pixel_concept = (alignof(Ty) == alignof(typename Ty::value_type)) && interleaved_pixel_concept<Ty>;

    namespace detail {
        // When doing with packet data, this could be very useful.
        template <std::integral Ty>
        struct packet_reference {
            Ty*          ptr;
            std::uint8_t beg, end;
            constexpr packet_reference& operator=(const Ty& value) {
                Ty cache = *ptr;
                *ptr = 0;
                *ptr |= (cache >> end) << end;                    // high.
                *ptr |= (value & ((1 << (end - beg)) - 1)) << beg; // middle.
                *ptr |= (cache & ((1 << beg) - 1));
                return *this;
            }
            constexpr operator const Ty() const { return (*ptr >> (beg)) & ((1 << (end - beg)) - 1); }
        };

        // 565 data layout's Int can only be short or unsigned short.
        template <std::integral Ty, std::size_t ... Sequence> requires (sizeof(Ty) == sizeof(std::uint16_t)) && (sizeof ... (Sequence) == 3)
        class alignas(std::uint16_t) int565_packed_pixel_t {
        public:
            using value_type = Ty;
            using reference = Ty&;
            using const_reference = const Ty&;

            constexpr int565_packed_pixel_t() = default;
            constexpr int565_packed_pixel_t(value_type a1, value_type a2, value_type a3) : mData(((a1 & 0x1F) << 11) | ((a2 & 0x3F) << 5) | (a3 & 0x1F)) {}
            constexpr int565_packed_pixel_t(const int565_packed_pixel_t&) = default;
            constexpr int565_packed_pixel_t(int565_packed_pixel_t&&) = default;

            constexpr int565_packed_pixel_t& operator=(const int565_packed_pixel_t&) = default;
            constexpr int565_packed_pixel_t& operator=(int565_packed_pixel_t&&) = default;

            constexpr std::size_t size() const { return 3; }
            constexpr decltype(auto) operator[](std::ptrdiff_t i)       { return get_ref(sAccessor[i]); }
            constexpr decltype(auto) operator[](std::ptrdiff_t i) const { return get_ref(sAccessor[i]); }

            constexpr operator value_type() const { return mData; }

            ~int565_packed_pixel_t() = default;
        private:
            constexpr decltype(auto) get_ref(std::size_t i) {
                std::uint8_t beg = 0, end = 0;
                switch (i) { case 0: end = 4; break; case 1: beg = 5; end = 10; break; case 2: beg = 11; end = 15; break; }
                return packet_reference{ &mData, beg, end };
            }
            constexpr decltype(auto) get_ref(std::size_t i) const {
                std::uint8_t beg = 0, end = 0;
                switch (i) { case 0: end = 4; break; case 1: beg = 5; end = 10; break; case 2: beg = 11; end = 15; break; }
                return packet_reference{ &mData, beg, end };
            }
            static constexpr std::array<std::size_t, 3> sAccessor = { Sequence... };
            value_type       mData;
        };
        template <typename Ty, std::size_t ... Sequence>
        class multichannel_pixel_t {
        public:
            using value_type = Ty;
            using reference = Ty&;
            using const_reference = const Ty&;

            constexpr multichannel_pixel_t() = default;
            template <typename ... Args> requires std::is_convertible_v<std::common_type_t<Args...>, value_type>
            constexpr multichannel_pixel_t(const Args ... args) : mData{ static_cast<value_type>(args)..., } {}
            constexpr multichannel_pixel_t(const multichannel_pixel_t&) = default;
            constexpr multichannel_pixel_t(multichannel_pixel_t&&) = default;
            constexpr multichannel_pixel_t& operator=(const multichannel_pixel_t&) = default;
            constexpr multichannel_pixel_t& operator=(multichannel_pixel_t&&) = default;

            constexpr std::size_t       size()                   const { return mData.size(); }
            constexpr reference         operator[](std::ptrdiff_t i) { return mData[sAccessor[i]]; }
            constexpr const_reference   operator[](std::ptrdiff_t i)      const { return mData[sAccessor[i]]; }
            constexpr operator vector<value_type, sizeof ...(Sequence)>() const { return mData; }
        private:
            static constexpr std::array<std::size_t, sizeof ...(Sequence)> sAccessor{ Sequence... };
            vector<value_type, sizeof ...(Sequence)> mData;
        };
    }

    using grey_u8_pixel_t     = detail::multichannel_pixel_t<std::uint8_t, 0>;
    using rgb565_u16_pixel_t  = detail::int565_packed_pixel_t<std::uint16_t, 2, 1, 0>;
    using bgr565_u16_pixel_t  = detail::int565_packed_pixel_t<std::uint16_t, 0, 1, 2>;
    using rgb565_s16_pixel_t  = detail::int565_packed_pixel_t<std::int16_t, 2, 1, 0>;
    using bgr565_s16_pixel_t  = detail::int565_packed_pixel_t<std::int16_t, 0, 1, 2>;
    using rgb888_u8_pixel_t   = detail::multichannel_pixel_t<std::uint8_t, 2, 1, 0>;
    using bgr888_u8_pixel_t   = detail::multichannel_pixel_t<std::uint8_t, 0, 1, 2>;
    using rgba8888_u8_pixel_t = detail::multichannel_pixel_t<std::uint8_t, 3, 2, 1, 0>;
    using abgr8888_u8_pixel_t = detail::multichannel_pixel_t<std::uint8_t, 0, 1, 2, 3>;
    // RGBA(ABGR) and ARGB(BGRA) shouldn't be placed in a single image format.
    using argb8888_u8_pixel_t = detail::multichannel_pixel_t<std::uint8_t, 3, 2, 1, 0>;
    using bgra8888_u8_pixel_t = detail::multichannel_pixel_t<std::uint8_t, 0, 1, 2, 3>;
}
