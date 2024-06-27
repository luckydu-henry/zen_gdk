///
/// \file      io_image_bmp.hpp
/// \brief     Contains methods to create and load bmp image.
/// \details
///
/// BMP codec (input and output)
/// For bmp whose depth is lower then 8, we don't have plan to support it, the RLE compression of
/// BMP is also not considered.
///
/// So this BMP codec is simply for image testing and study, use TGA or JPEG or PNG
/// if you want to have a rich featured image codec (more color format and compression algorithm).
/// Current status:
/// 8bit  bmp -- partial supported (having issues with row access)
/// 16bit bmp -- fully supported.
/// 24bit bmp -- fully supported.
/// 32bit bmp -- fully supported.
///
/// \author    HenryDu
/// \date      24.06.2024
/// \copyright © HenryDu 2024. All right reserved.
///
#pragma once

#include <algorithm>
#include <istream>
#include <ostream>

#include "force/media/utility.hpp"
#include "force/media/image_view.hpp"
namespace force::media::io {

    // Three color kinds of bmp that is supported by the codec, which one would be used is decided by
    // image's depth(8, 16, 24, 32)
    using image_bmp_interleaved_view = image_variant_interleaved_view<
        grey_u8_pixel_t,
        bgr565_u16_pixel_t,
        bgr888_u8_pixel_t,
        bgra8888_u8_pixel_t
    >;

    struct image_bmp {
        image_bmp_interleaved_view view;
        std::uint8_t         padding;
        std::uint8_t         depth; // 8, 16, 24, 32.
    };

    image_bmp make_image_bmp_from_raw(std::byte* ptr, std::size_t w, std::size_t h, std::uint8_t depth) {
        image_bmp bmp;
        bmp.depth = depth;
        switch (bmp.depth >> 3) {
        case 1: bmp.view = make_image_variant_interleaved_view<image_bmp_interleaved_view>(reinterpret_cast<grey_u8_pixel_t*>(ptr), w, h, w);
                bmp.padding = (w & 3) == 0 ? 0 : 4 - w & 3; break;
        case 2: bmp.view = make_image_variant_interleaved_view<image_bmp_interleaved_view>(reinterpret_cast<bgr565_u16_pixel_t*>(ptr), w, h, w);
                bmp.padding = (w & 1) == 0 ? 0 : 2; break;
        case 3: bmp.view = make_image_variant_interleaved_view<image_bmp_interleaved_view>(reinterpret_cast<bgr888_u8_pixel_t*>(ptr), w, h, w);
                bmp.padding = w & 3; break;
        case 4: bmp.view = make_image_variant_interleaved_view<image_bmp_interleaved_view>(reinterpret_cast<bgr888_u8_pixel_t*>(ptr), w, h, w);
                bmp.padding = 0; break;
        default:bmp.padding = -1; break;
        }
        return bmp;
    }
    // f takes in the active view.
    template <typename Fn>
    constexpr decltype(auto) visit_image_bmp_interleaved_view(image_bmp_interleaved_view view, std::uint8_t depth, Fn f) {
        switch (depth >> 3) {
        case 1: return std::invoke(f, view.get<grey_u8_pixel_t>());
        case 2: return std::invoke(f, view.get<bgr565_u16_pixel_t>());
        case 3: return std::invoke(f, view.get<bgr888_u8_pixel_t>());
        case 4: return std::invoke(f, view.get<bgra8888_u8_pixel_t>());
        }
    }
    template <interleaved_pixel_concept Px>
    image_bmp make_image_bmp_from_view(image_interleaved_view<Px> view) {
        image_bmp bmp{view, 0, sizeof(Px) << 3};
        std::uint8_t w = view.width();
        switch (bmp.depth >> 3) {
        case 1: bmp.padding = (w & 3) == 0 ? 0 : 4 - w & 3; break;
        case 2: bmp.padding = (w & 1) == 0 ? 0 : 2; break;
        case 3: bmp.padding = w & 3; break;
        case 4: bmp.padding = 0; break;
        default: bmp.padding = -1;
        }
        return bmp;
    }
    template <typename Fn>
    constexpr decltype(auto) visit_image_bmp(image_bmp& bmp, Fn f) {
        return visit_image_bmp_interleaved_view(bmp.view, bmp.depth, f);
    }
    template <typename Fn>
    constexpr decltype(auto) visit_image_bmp(const image_bmp& bmp, Fn f) {
        return visit_image_bmp_interleaved_view(bmp.view, bmp.depth, f);
    }
    inline std::size_t image_bmp_byte_size(const image_bmp& bmp) {
        return visit_image_bmp(bmp, [](auto& v) { return v.width(); }) * visit_image_bmp(bmp, [](auto& v) { return v.height(); }) * bmp.depth >> 3;
    }

    /// \brief  A bmp reader which returns the information you need for dynamically handle bmp in memory
    /// \param  allocator - Used to dynamic allocate your memory.
    /// \param  ptr       - Where the data will be stored.
    /// \param  ips       - Input stream a file or a byte sequence is ok.
    /// \retval           - Contains information you need to handle a bmp.
    image_bmp read_image_bmp_from_stream(std::byte* ptr, std::istream& ips, std::pmr::polymorphic_allocator<std::byte> allocator = std::pmr::polymorphic_allocator()) {

        if (!ips.good()) {
            throw std::runtime_error("Stream error!");
        }
        std::byte bmpInfo[54] = {};
        ips.read((char*)bmpInfo, sizeof(bmpInfo));

        auto  begOffset    = get_int_from_buffer<int32_t> (bmpInfo, 10);
        auto  width        = get_int_from_buffer<int32_t> (bmpInfo, 18);
        auto  height       = get_int_from_buffer<int32_t> (bmpInfo, 22);
        auto  depth        = get_int_from_buffer<uint16_t>(bmpInfo, 28);
        auto  compression  = get_int_from_buffer<uint8_t> (bmpInfo, 30);

        ptr = allocator.allocate((width * height * depth + 7) >> 3);

        // Not BI_RGB or BI_BITFIELD(RGBA)
        if (compression != 0 and compression != 3) {
            throw std::runtime_error("Error, invalid bmp color tag");
        }

        // Jump to actual bit map data.
        ips.seekg(begOffset, std::ios::beg);

        // Decide padding mode.
        image_bmp   img = make_image_bmp_from_raw(ptr, width, height, depth);
        // For access in right sequence.
        visit_image_bmp(img, [&img](auto& v) { img.view = reverse_col_view(v); });

        matrix_view view(ptr, 0, 0, width, height, width * depth >> 3, depth >> 3);
        // Read data into raw view
        for_each_view(view, [&ips, &view](auto& v) {ips.read(reinterpret_cast<char*>(&v), view.col_delta());}, 
                            [&ips, &img] (auto& v) {ips.seekg(img.padding, std::ios::cur);});
        return img;
    }
    void write_image_bmp_to_stream(const image_bmp& img, std::ostream& ops) {
        if (!ops.good()) {
            throw std::runtime_error("Error stream is not in good bit!");
        }
        image_bmp_interleaved_view view;
        visit_image_bmp(img, [&view](auto& v) {view = reverse_col_view(v); });

        // All BMP written by Terse functions are without color pallete.
        std::byte bmpInfo[54] = {};
        std::fill_n(bmpInfo, sizeof(bmpInfo), static_cast<std::byte>(0));

        if (img.padding == -1) {
            throw std::runtime_error("Error, invalid bits-per-pixel format!");
        }

        auto width  = visit_image_bmp(img, [](auto& v) { return v.width(); });
        auto height = visit_image_bmp(img, [](auto& v) { return v.height(); });
        auto channel   = img.depth >> 3;
        auto padding = img.padding;

        // Set BMP info.
        put_int_to_buffer<uint16_t>(bmpInfo, 0, 0x4D42);                                    
        put_int_to_buffer<uint32_t>(bmpInfo, 2, width * height * channel + height * padding + 54);
        put_int_to_buffer<uint8_t> (bmpInfo, 10, 54);                                        
        put_int_to_buffer<uint8_t> (bmpInfo, 14, 40);
        put_int_to_buffer<int32_t> (bmpInfo, 18, width);
        put_int_to_buffer<int32_t> (bmpInfo, 22, height);
        put_int_to_buffer<uint16_t>(bmpInfo, 26, 1);
        put_int_to_buffer<uint16_t>(bmpInfo, 28, (channel << 3) & 0xFFFF);
        put_int_to_buffer<int8_t>  (bmpInfo, 30, 0);
        put_int_to_buffer<uint32_t>(bmpInfo, 34, width * height * channel);

        ops.write(reinterpret_cast<char*>(bmpInfo), sizeof(bmpInfo));

        visit_image_bmp_interleaved_view(view, img.depth, [&ops, channel, padding](auto& view) {
            for_each_view(view, [&ops, channel](const auto& p) {
                auto* beg = reinterpret_cast<const std::byte*>(&p);
                std::byte b[4];
                std::copy_n(beg, channel, b);
                ops.write(reinterpret_cast<char*>(b), channel);
            }, [&ops, padding](const auto&) {
                std::byte b[4];
                ops.write(reinterpret_cast<char*>(b), padding);
            });
        });
    }
}