///
/// \file      image_view.hpp
/// \brief     Specialization of matrix_view so that it can be used with images.
/// \details   ~
/// \author    HenryDu
/// \date      24.06.2024
/// \copyright © HenryDu 2024. All right reserved.
///
#pragma once

#include <utility>
#include <variant>

#include "force/matrix_view.hpp"
#include "force/media/pixels.hpp"
namespace force::media {
    template <interleaved_pixel_concept Pix>
    using image_interleaved_view = matrix_view<Pix>;

    template <typename ... PlanarPix>
    class image_planar_view {
    public:
        constexpr image_planar_view() = default;
        constexpr image_planar_view(matrix_view<PlanarPix>... views) : mPlanes{views...}{}
        constexpr image_planar_view(const image_planar_view&) = default;
        constexpr image_planar_view(image_planar_view&&)      = default;

        image_planar_view& operator=(const image_planar_view&) = default;
        image_planar_view& operator=(image_planar_view&&)      = default;

        constexpr std::size_t    planar_count() const { return sizeof ... (PlanarPix); }
        template <std::size_t Id>
        constexpr decltype(auto) get()       { return std::get<Id>(mPlanes); }
        template <std::size_t Id>
        constexpr decltype(auto) get() const { return std::get<Id>(mPlanes); }

        ~image_planar_view() = default;
    private:
        std::tuple<matrix_view<PlanarPix>...> mPlanes;
    };
    // For dynamic image.
    template <interleaved_pixel_concept ... Pix>
    class image_variant_interleaved_view {
    public:
        constexpr image_variant_interleaved_view():mViews(){}
        template <interleaved_pixel_concept Ty>
        constexpr image_variant_interleaved_view(const image_interleaved_view<Ty> view) : mViews(view) {}
        constexpr image_variant_interleaved_view(const image_variant_interleaved_view&) = default;
        constexpr image_variant_interleaved_view(image_variant_interleaved_view&&)      = default;

        constexpr image_variant_interleaved_view& operator=(const image_variant_interleaved_view&) = default;
        constexpr image_variant_interleaved_view& operator=(image_variant_interleaved_view&&)      = default;

        template <interleaved_pixel_concept Ty>
        constexpr decltype(auto) get()       { return std::get<image_interleaved_view<Ty>>(mViews); }
        template <interleaved_pixel_concept Ty>
        constexpr decltype(auto) get() const { return std::get<image_interleaved_view<Ty>>(mViews); }

        ~image_variant_interleaved_view() = default;
    private:
        std::variant<image_interleaved_view<Pix>...> mViews;
    };
    /// \brief  
    /// \tparam VariantInterleavedView - A image_variant_interleaved alias.
    /// \param  data                   - Pointer which pointed to actual data.
    /// \example
    /// using image_bmp_interleaved_view = image_variant_interleaved_view<bgr565_u16_pixel_t, rgba8888_u8_pixel_t, bgr888_u8_pixel_t>;
    /// auto view = make_image_variant_interleaved_view<image_bmp_interleaved_view>(...);
    /// \retval                        - return a image_variant_interleaved_view.
    template <class VariantInterleavedView, interleaved_pixel_concept Pix>
    constexpr decltype(auto) make_image_variant_interleaved_view(Pix* data, std::size_t w, std::size_t h, std::size_t rowsize) {
        return VariantInterleavedView(image_interleaved_view<Pix>(data, 0, 0, w, h, rowsize));
    }
    template <class VariantInterleavedView, interleaved_pixel_concept Pix>
    constexpr decltype(auto) copy_variant_interleaved_view(const VariantInterleavedView view, Pix* dest) {
        copy_view(view.template get<Pix>(), dest);
    }
}