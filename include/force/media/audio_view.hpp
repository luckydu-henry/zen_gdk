///
/// \file      audio_view.hpp
/// \brief     A view for dealing with audios
/// \details   ~
/// \author    HenryDu
/// \date      25.06.2024
/// \copyright © HenryDu 2024. All right reserved.
///
#pragma once

#include "force/vector_view.hpp"
#include "force/vector.hpp"

namespace force::media {
    // Define three kinds of sample dots.
    template <arithmetic Precision, std::size_t Channel>
    using audio_sample  = vector<Precision, Channel>;
    template <arithmetic Precision>
    using mono_sample   = audio_sample<Precision, 1>;
    template <arithmetic Precision>
    using stereo_sample = audio_sample<Precision, 2>;

    // These samples may be frequently used under real scenes.
    using mono_u8_sample  = mono_sample<std::uint8_t>;
    using mono_s8_sample  = mono_sample<std::int8_t>;
    using mono_u16_sample = mono_sample<std::uint16_t>;
    using mono_s16_sample = mono_sample<std::int16_t>;
    using mono_f32_sample = mono_sample<float>;

    using stereo_u8_sample  = stereo_sample<std::uint8_t>;
    using stereo_s8_sample  = stereo_sample<std::int8_t>;
    using stereo_u16_sample = stereo_sample<std::uint16_t>;
    using stereo_s16_sample = stereo_sample<std::int16_t>;
    using stereo_f32_sample = stereo_sample<float>;

    namespace detail {
        template <typename Ty>
        struct is_audio_sample : std::false_type {};
        template <typename Ty, std::size_t N>
        struct is_audio_sample<audio_sample<Ty, N>> : std::true_type {};
    }

    template <typename Ty>
    concept audio_sample_concept = detail::is_audio_sample<Ty>::value;

    // Quite useful when streaming, or you want to batch handle samples.
    // Time unit (1 second) or Space unit (1 frame)
    template <audio_sample_concept Sample, std::size_t Num>
    using sample_frame_buffer = vector<Sample, Num>;
    // A few aliases for you to use in case you are not very confused.
    using frame_buffer_aac_classic = sample_frame_buffer<mono_s16_sample, 1024>;
    using frame_buffer_mp3_classic = sample_frame_buffer<mono_s16_sample, 1152>;

    template <audio_sample_concept Sample>
    using audio_interleaved_view = vector_view<Sample>;

    template <audio_sample_concept ... Samples>
    class audio_variant_interleaved_view {
    public:
        constexpr audio_variant_interleaved_view() :mViews() {}
        template <interleaved_pixel_concept Ty>
        constexpr audio_variant_interleaved_view(const audio_interleaved_view<Ty> view) : mViews(view) {}
        constexpr audio_variant_interleaved_view(const audio_variant_interleaved_view&) = default;
        constexpr audio_variant_interleaved_view(audio_variant_interleaved_view&&) = default;

        constexpr audio_variant_interleaved_view& operator=(const audio_variant_interleaved_view&) = default;
        constexpr audio_variant_interleaved_view& operator=(audio_variant_interleaved_view&&) = default;

        template <audio_sample_concept Ty>
        constexpr decltype(auto) get()       { return std::get<audio_interleaved_view<Ty>>(mViews); }
        template <audio_sample_concept Ty>
        constexpr decltype(auto) get() const { return std::get<audio_interleaved_view<Ty>>(mViews); }

        ~audio_variant_interleaved_view() = default;
    private:
        std::variant<audio_interleaved_view<Samples>...> mViews;
    };


    template <class VariantInterleavedView, audio_sample_concept Sample>
    constexpr decltype(auto) make_audio_variant_interleaved_view(Sample* data, std::size_t sample_count) {
        return VariantInterleavedView(audio_interleaved_view<Sample>(data, 0, sample_count));
    }
    template <audio_sample_concept Sample>
    struct for_each_frame_result {
        std::size_t    count;   // How many units were processed.
        Sample*        ptr;     // Begin position of remained samples.
        std::ptrdiff_t remain;  // How many samples are remained.
    };
    template <std::size_t Lambda, audio_sample_concept Sample, typename Fn>
    constexpr decltype(auto) for_each_frame(audio_interleaved_view<Sample> view, Fn f) {
        auto count      = view.size() / Lambda;
        auto ptr        = view.data() + count * Lambda;
        auto remain     = view.size() % Lambda;
        auto unit_view  = vector_view(reinterpret_cast<sample_frame_buffer<Sample, Lambda>>(view.data()), 0, count);
        for (auto& unit: unit_view) {
            std::invoke(f, unit);
        }
        return for_each_frame_result{ count, ptr, remain };
    }

}