///
/// \file      audio_algorithm_sample.hpp
/// \brief     It is used to create a audio view from various sample algorithms.
/// \details   ~
/// \author    HenryDu
/// \date      26.06.2024
/// \copyright © HenryDu 2024. All right reserved.
///
#pragma once

#include <chrono>
#include <thread>

#include "force/media/audio_view.hpp"

namespace force::media {
    template <floating_point Precision>
    constexpr auto tet_factor = static_cast<Precision>(1.05946309435929526456182); // 2^(1/12)
    // Using standard A's to generate all other keys.
    template <floating_point Precision>
    constexpr auto tet_tone_a8 = static_cast<Precision>(7040);
    template <floating_point Precision>
    constexpr auto tet_tone_a7 = static_cast<Precision>(3520);
    template <floating_point Precision>
    constexpr auto tet_tone_a6 = static_cast<Precision>(1760);
    template <floating_point Precision>
    constexpr auto tet_tone_a5 = static_cast<Precision>(880);
    template <floating_point Precision>
    constexpr auto tet_tone_a4 = static_cast<Precision>(440);
    template <floating_point Precision>
    constexpr auto tet_tone_a3 = static_cast<Precision>(220);
    template <floating_point Precision>
    constexpr auto tet_tone_a2 = static_cast<Precision>(110);
    template <floating_point Precision>
    constexpr auto tet_tone_a1 = static_cast<Precision>(55);

    /// \brief  Generate a key on the piano quickly.
    /// \tparam Precision - Higher precision means higher quality.
    /// \param  base_tone - The base key
    /// \param  half_key  - How much half key since base key
    /// \example
    /// // Create C4 key
    /// auto c4 = make_tet_tone(tet_tone_a4<float>, -9); // 261Hz.
    /// \retval           - The key displaced.
    template <floating_point Precision>
    constexpr decltype(auto) make_tet_tone(Precision base_tone, std::ptrdiff_t half_key) {
        if (half_key > 0) { for (std::ptrdiff_t i = 0; i != half_key; ++i) { base_tone *= tet_factor<Precision>; } return base_tone; }
        if (half_key < 0) { for (std::ptrdiff_t i = 0; i != half_key; --i) { base_tone /= tet_factor<Precision>; } return base_tone; }
        return base_tone;
    }
    // Oscillator is a unary function of time.
    // All output must within range [-1, 1]
    // Default is single channel.
    template <floating_point Precision, audio_sample_concept Sample, typename Oscillator, class Rep1, class Period1, class Rep2, class Period2>
    constexpr decltype(auto) sample_interleaved_from_oscillator(audio_interleaved_view<Sample> view, 
                                                                std::chrono::duration<Rep1, Period1> start,
                                                                std::chrono::duration<Rep2, Period2> stop,
                                                                std::size_t frequency, Oscillator S) {
        using value_type      = typename Sample::value_type;
        using sample_duration = std::chrono::duration<Precision, std::ratio<1>>; // Convert to floating point seconds.
        auto ti = std::chrono::duration_cast<sample_duration>(start).count();
        auto tf = std::chrono::duration_cast<sample_duration>(stop).count();
        auto dt = static_cast<Precision>(1) / static_cast<Precision>(frequency);
        auto it = view.begin();
        // tf doesn't count
        for (; it != view.end() && ((tf - ti) > std::numeric_limits<Precision>::epsilon()); ++it, ti += dt) {
            Precision s = std::invoke(S, ti);
            *it = floating_point<value_type> ? value_type(s) : value_type(s * Precision(std::numeric_limits<value_type>::max()));
        }
        return it;
    }
}