///
/// \file      io_image_tga.hpp
/// \brief     A codec for truevision targa image.
/// \details
///
/// TGA support status:
/// 8bit   -- fully supported.
/// 16bit  -- partial (RLE not supported)
/// 24bit  -- fully supported.
/// 32bit  -- fully supported.
/// Index image is not supported.
///
/// \author    HenryDu
/// \date      25.06.2024
/// \copyright © HenryDu 2024. All right reserved.
///
#pragma once

#include <algorithm>
#include <ranges>
#include <istream>
#include <ostream>

#include "force/media/utility.hpp"
#include "force/media/image_view.hpp"

namespace force::media::io {
    using image_tga_interleaved_view = image_variant_interleaved_view<
        grey_u8_pixel_t,
        // 16 bit is wait for supported.
        bgr888_u8_pixel_t,
        bgra8888_u8_pixel_t
    >;
    struct image_tga {
        image_tga_interleaved_view view;
        std::uint8_t               depth;
    };

    image_tga read_tga_from_stream(std::byte* ptr, std::istream& ips, std::pmr::polymorphic_allocator<std::byte> allocator = std::pmr::polymorphic_allocator()) {

        if (!ips.good()) {
            throw std::runtime_error("Input stream is not in good bit!");
        }
        // Defines the tgaInfo info.
        std::byte tgaInfo[18] = {};
        ips.read(reinterpret_cast<char*>(tgaInfo), sizeof(tgaInfo));

        auto   idLength = get_int_from_buffer<uint8_t>(tgaInfo, 0);
        auto   cMapType = get_int_from_buffer<uint8_t>(tgaInfo, 1);
        auto   imagType = get_int_from_buffer<uint8_t>(tgaInfo, 2);
        // Color map specification.
        // TODO: Add color map support for tga file format.
        // auto   fEntryId = get_from_buf<uint16_t>(tgaInfo, 3);
        // auto   cMapLnth = get_from_buf<uint16_t>(tgaInfo, 5);
        // auto   cEntrySz = get_from_buf<uint8_t> (tgaInfo, 7);

        // Image specification.
        auto   width   = detail::get_int_from_buffer<uint16_t>(tgaInfo, 12);
        auto   height  = detail::get_int_from_buffer<uint16_t>(tgaInfo, 14);
        auto   depth   = detail::get_int_from_buffer<uint8_t> (tgaInfo, 16);

        if (imagType == 0 or imagType == 1 or imagType == 9) {
            throw std::runtime_error("Not support color map image!");
        }
        if (idLength != 0) {
            throw std::runtime_error("Why you need an id length? This is a tga2.(put meta info to the extension area).");
        }
        if (cMapType != 0 or depth == (2 << 3)) {
            throw std::runtime_error("Not support color map image!");
        }
        ptr = allocator.allocate(width * height * depth >> 3);





    }
    void write_image_bmp_to_stream(const image_tga& img, std::ostream& ops) {
        
    }
}
