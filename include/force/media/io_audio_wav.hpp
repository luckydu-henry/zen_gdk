///
/// \file      io_audio_wav.hpp
/// \brief     A codec for wave format.
/// \details
///
/// Fully featured standard RIFF wave format codec
/// To use this you must make sure your wave file header is exactly 44 bytes.
/// Also this writer also writes wave header in exactly 44 bytes header.
///
/// \author    HenryDu
/// \date      26.06.2024
/// \copyright © HenryDu 2024. All right reserved.
///
#pragma once

#include <istream>
#include <ostream>

#include "utility.hpp"
#include "audio_view.hpp"

namespace force::media::io {
    using audio_wave_interleaved_view = audio_variant_interleaved_view<
        mono_s8_sample,
        mono_s16_sample,
        mono_f32_sample,
        stereo_s8_sample,
        stereo_s16_sample,
        stereo_f32_sample
    >;

    struct audio_wave {
        audio_wave_interleaved_view view;
        std::uint8_t               depth;
        std::uint8_t               channel;
        std::size_t                frequency;
    };

    audio_wave make_audio_wav_from_raw(std::byte* ptr, std::uint8_t depth, std::uint8_t channel, std::size_t sample_count, std::size_t frequency) {
        audio_wave wave{ .depth = depth, .channel = channel ,.frequency = frequency};
        switch (depth) {
        case 8:
            switch (channel) {
            case 1: wave.view = make_audio_variant_interleaved_view<audio_wave_interleaved_view>(reinterpret_cast<mono_s8_sample*>(ptr), sample_count); break;
            case 2: wave.view = make_audio_variant_interleaved_view<audio_wave_interleaved_view>(reinterpret_cast<stereo_s8_sample*>(ptr), sample_count); break;}
            break;
        case 16:
            switch(channel) {
            case 1: wave.view = make_audio_variant_interleaved_view<audio_wave_interleaved_view>(reinterpret_cast<mono_s16_sample*>(ptr), sample_count); break;
            case 2: wave.view = make_audio_variant_interleaved_view<audio_wave_interleaved_view>(reinterpret_cast<stereo_s16_sample*>(ptr), sample_count); break; }
            break;
        case 32:
            switch (channel) {
            case 1: wave.view = make_audio_variant_interleaved_view<audio_wave_interleaved_view>(reinterpret_cast<mono_f32_sample*>(ptr), sample_count); break;
            case 2: wave.view = make_audio_variant_interleaved_view<audio_wave_interleaved_view>(reinterpret_cast<stereo_f32_sample*>(ptr), sample_count); break; }
            break;
        }
        return wave;
    }
    template <typename Fn>
    constexpr decltype(auto) visit_audio_wave_view(audio_wave_interleaved_view view, std::uint8_t depth, std::uint8_t channel, Fn f) {
        switch (depth) {
        case 8:
            switch (channel) {
            case 1: return std::invoke(f, view.get<mono_s8_sample>());
            case 2: return std::invoke(f, view.get<stereo_s8_sample>());}
            break;
        case 16:
            switch (channel) {
            case 1: return std::invoke(f, view.get<mono_s16_sample>());
            case 2: return std::invoke(f, view.get<stereo_s16_sample>()); }
            break;
        case 32:
            switch (channel) {
            case 1: return std::invoke(f, view.get<mono_f32_sample>());
            case 2: return std::invoke(f, view.get<stereo_f32_sample>()); }
            break;
        }
    }
    template <typename Fn>
    constexpr decltype(auto) visit_audio_wave(audio_wave& wav, Fn f) {
        return visit_audio_wave_view(wav.view, wav.depth, wav.channel, f);
    }
    template <typename Fn>
    constexpr decltype(auto) visit_audio_wave(const audio_wave& wav, Fn f) {
        return visit_audio_wave_view(wav.view, wav.depth, wav.channel, f);
    }
    template <audio_sample_concept Sample>
    audio_wave make_audio_wave_from_view(audio_interleaved_view<Sample> view, std::size_t frequency) {
        return audio_wave(view, sizeof(Sample::value_type) << 3, Sample::num_dimensions, frequency);
    }
    inline std::size_t audio_wave_byte_size(const audio_wave& wav) {
        return visit_audio_wave(wav, [](auto& v) { return v.size(); }) * wav.channel * wav.depth >> 3;
    }
    // Filestream I/O.

    namespace detail {
        inline std::tuple<std::uint16_t, std::uint16_t, std::uint16_t, std::uint32_t>
        get_audio_wave_header_info(std::byte* wave_header) {
            return std::make_tuple(
                get_int_from_buffer<std::uint16_t>(wave_header, 0x16),
                get_int_from_buffer<std::uint32_t>(wave_header, 0x18),
                get_int_from_buffer<std::uint16_t>(wave_header, 0x22),
                get_int_from_buffer<std::uint32_t>(wave_header, 0x28)
            );
        }

        inline void
        put_audio_wave_header_info(const std::tuple<std::uint16_t, std::uint16_t, std::uint16_t, std::uint32_t>& info, std::byte* wave_header) {
            auto [channel, frequency, depth, size] = info;
            put_int_to_buffer<std::uint32_t>(wave_header, 0x00, 0x46464952);
            put_int_to_buffer<std::uint32_t>(wave_header, 0x04, size + 36);
            put_int_to_buffer<std::uint32_t>(wave_header, 0x08, 0x45564157);
            put_int_to_buffer<std::uint32_t>(wave_header, 0x0C, 0x20746D66);
            put_int_to_buffer<std::uint32_t>(wave_header, 0x10, 16);
            put_int_to_buffer<std::uint16_t>(wave_header, 0x14, 1);
            put_int_to_buffer<std::uint16_t>(wave_header, 0x16, channel);
            put_int_to_buffer<std::uint32_t>(wave_header, 0x18, frequency);
            put_int_to_buffer<std::uint32_t>(wave_header, 0x1C, (frequency * channel * depth >> 3));
            put_int_to_buffer<std::uint16_t>(wave_header, 0x20, (depth * channel >> 3));
            put_int_to_buffer<std::uint16_t>(wave_header, 0x22, depth);
            put_int_to_buffer<std::uint32_t>(wave_header, 0x24, 0x61746164);
            put_int_to_buffer<std::uint32_t>(wave_header, 0x28, size);
        }
    }

    // This one get all information to memory, however you can use audio streamer to handle image without much memory waste.
    // You might want to use this when dealing with effects, but not use this when you are dealing with music (Large chunk of data.)
    audio_wave read_audio_wave_from_stream(std::byte* ptr, std::istream& ips, std::pmr::polymorphic_allocator<> allocator = std::pmr::polymorphic_allocator<>{}) {
        if (!ips.good()) {
            throw std::runtime_error("Input stream does not have good bit.");
        }
        std::byte wave_header[44];
        ips.read(reinterpret_cast<char*>(wave_header), sizeof(wave_header));
        // Get information by wave header.
        auto [channel, frequency, depth, size] = detail::get_audio_wave_header_info(wave_header);
        // Allocate all samples in one time
        ptr = allocator.allocate(size);
        ips.read(reinterpret_cast<char*>(ptr), size);
        // (depth / 8) * channel * count = size.
        return make_audio_wav_from_raw(ptr, depth, channel, ((size << 3) / depth / channel), frequency);
    }

    void      write_audio_wave_to_stream(const audio_wave& wave, std::ostream& ops) {

        if (!ops.good()) {
            throw std::runtime_error("Output stream does not have good bit.");
        }

        auto size_bytes = audio_wave_byte_size(wave);

        std::byte wave_header[44];
        detail::put_audio_wave_header_info(std::make_tuple(wave.channel, wave.frequency, wave.depth, size_bytes), wave_header);
        ops.write(reinterpret_cast<char*>(wave_header), sizeof(wave_header));

        visit_audio_wave(wave, [&ops](auto& view) {
            using          value_type     = typename std::remove_cvref_t<decltype(view)>::value_type;
            static constexpr auto num_dimensions = value_type::num_dimensions;
            for_each_view(view, [&ops](auto& value) {
                ops.write(reinterpret_cast<char*>(value.data()), num_dimensions * sizeof(value_type) * value.size());
            });
        });
    }

}
