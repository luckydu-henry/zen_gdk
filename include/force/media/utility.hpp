///
/// \file      utility.hpp
/// \brief     Contains some common utilities to deal with media.
/// \details   ~
/// \author    HenryDu
/// \date      25.06.2024
/// \copyright © HenryDu 2024. All right reserved.
///
#pragma once

#include <concepts>
#include <bit>

namespace force::media::io {

    // Two functions that can make things easier.
// We don't want to see so many bit operation in our real code.
    template <std::integral Int>
    Int get_int_from_buffer(std::byte* buf, std::size_t startIdx) {
        Int res = 0; // Uses little endian.
        for (Int i = 0; i < static_cast<Int>(sizeof(Int)); ++i) { res |= static_cast<Int>(buf[startIdx + i]) << (i << 3); }
        return res;
    }
    template <std::integral Ty>
    void put_int_to_buffer(std::byte* buf, std::size_t startIdx, Ty data) {
        // Uses little endian.
        for (std::size_t i = 0; i < sizeof(Ty); ++i) { buf[startIdx + i] = static_cast<std::byte>((data >> (i << 3)) & 0xFF); }
    }
}