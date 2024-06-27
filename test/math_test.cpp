#include <algorithm>
#include <print>
#include <vector>
#include <iostream>
#include <numeric>
#include <ranges>
#include <fstream>
#include <any>

#include "force/matrix.hpp"
#include "force/matrix_view.hpp"
#include "force/vector_view.hpp"
#include "force/vector.hpp"
#include "force/complex.hpp"
#include "force/media/pixels.hpp"
#include "force/media/image_view.hpp"
#include "force/media/io_image_bmp.hpp"
#include "force/media/audio_view.hpp"
#include "force/media/io_audio_wav.hpp"
#include "force/media/audio_algorithm_sample.hpp"


// Demo -- Little star with simple oscillators.
//int main(int argc, char* argv[]) {
//
//    namespace fmd = force::media;
//    using namespace std::chrono_literals;
//
//    std::array<fmd::mono_s16_sample, 8000> buffer;
//
//    struct sine_wave_oscillator {
//        std::ptrdiff_t i;
//        decltype(auto) operator()(float t) const {
//            static constexpr auto two_pi = 2.F * std::numbers::pi_v<float>;
//            const auto tone = fmd::make_tet_tone(fmd::tet_tone_a4<float>, i);
//            return force::sin(two_pi * tone * t);
//        }
//    };
//
//    struct square_wave_oscillator {
//        std::ptrdiff_t i;
//        decltype(auto) operator()(float t) const {
//            static constexpr auto two_pi = 2.F * std::numbers::pi_v<float>;
//            const auto tone = fmd::make_tet_tone(fmd::tet_tone_a4<float>, i);
//            // Square wave equation.
//            return force::sgn(force::sin(two_pi * tone * t));
//        }
//    };
//
//    struct triangular_wave_oscillator {
//        std::ptrdiff_t i;
//        decltype(auto) operator()(float t) const {
//            static constexpr auto pi = std::numbers::pi_v<float>;
//            const auto tone = fmd::make_tet_tone(fmd::tet_tone_a4<float>, i);
//            // Triangular wave equation.
//            return 2.F * force::asin(force::sin((2.F * pi) * tone * t)) / pi;
//        }
//    };
//
//    struct sawtooth_wave_oscillator {
//        std::ptrdiff_t i;
//        decltype(auto) operator()(float t) const {
//            static constexpr auto pi = std::numbers::pi_v<float>;
//            const auto tone = fmd::make_tet_tone(fmd::tet_tone_a4<float>, i);
//            // Triangular wave equation.
//            return 2.F * force::asin(force::sin(pi * tone * t)) / pi * force::sgn(force::cos(pi * tone * t));
//        }
//    };
//
//    std::ofstream ofs("test.wav", std::ios::binary);
//    std::byte wave_header[44];
//    fmd::io::detail::put_audio_wave_header_info(std::make_tuple(1, 8000, 16, 8000 * 2 * 32), wave_header);
//    ofs.write(reinterpret_cast<char*>(wave_header), sizeof(wave_header));
//
//    static constexpr auto c4   = -9;
//    static constexpr auto d4   = -7;
//    static constexpr auto e4   = -5;
//    static constexpr auto f4   = -4;
//    static constexpr auto g4   = -2;
//    static constexpr auto a4   = 0;
//    static constexpr auto b4   = 2;
//    static constexpr auto fs4  = -3;
//
//
//    auto write_tone = [&](std::ptrdiff_t note) {
//        fmd::audio_interleaved_view view(buffer.data(), 0, 8000);
//        fmd::sample_interleaved_from_oscillator<float>(view, 0ms, 1s, 8000, sawtooth_wave_oscillator{ note });
//        ofs.write(reinterpret_cast<char*>(view.data()), sizeof(buffer) >> 1);
//    };
//
//    auto write_break = [&]() {
//        std::fill_n(buffer.data(), 8000, force::media::mono_s16_sample());
//        ofs.write(reinterpret_cast<char*>(buffer.data()), sizeof(buffer) >> 1);
//    };
//
//    write_tone(c4);
//    write_tone(c4);
//    write_tone(g4);
//    write_tone(g4);
//    write_tone(a4);
//    write_tone(a4);
//    write_tone(g4);
//    write_break();
//
//    write_tone(f4);
//    write_tone(f4);
//    write_tone(e4);
//    write_tone(e4);
//    write_tone(d4);
//    write_tone(d4);
//    write_tone(c4);
//    write_break();
//
//    write_tone(g4);
//    write_tone(g4);
//    write_tone(f4);
//    write_tone(f4);
//    write_tone(e4);
//    write_tone(e4);
//    write_tone(d4);
//    write_break();
//
//    write_tone(g4);
//    write_tone(g4);
//    write_tone(f4);
//    write_tone(f4);
//    write_tone(e4);
//    write_tone(e4);
//    write_tone(d4);
//    write_break();
//
//    write_tone(c4);
//    write_tone(c4);
//    write_tone(g4);
//    write_tone(g4);
//    write_tone(a4);
//    write_tone(a4);
//    write_tone(g4);
//    write_break();
//
//    write_tone(f4);
//    write_tone(f4);
//    write_tone(e4);
//    write_tone(e4);
//    write_tone(d4);
//    write_tone(d4);
//    write_tone(c4);
//    write_break();
//
//    ofs.close();
//}

// Demo image process.
//int main(int argc, char* argv[]) {
//    namespace fmd = force::media;
//
//    static constexpr std::size_t img_size = 100 * 100 * 3;
//
//    std::byte*  img_data = std::pmr::polymorphic_allocator<>().allocate(img_size);
//    std::fill_n(img_data, img_size, static_cast<std::byte>(0));
//    
//    auto view = fmd::image_interleaved_view(reinterpret_cast<fmd::bgr888_u8_pixel_t*>(img_data), 0, 0, 100, 100, 100);
//    
//    for (std::size_t i = 0; i != view.width(); ++i) {
//        auto v = view.col_at(i);
//        for (std::size_t j = 0; j != view.height(); ++j) {
//            v[j][2] = 255 * i / view.height(); // Blue.
//            v[j][0] = 255 * j / view.height(); // Green.
//        }
//    }
//
//    // view = force::rotate_view_half_pi(view);
//    auto bmp = fmd::io::make_image_bmp_from_view(view);
//    
//    std::ofstream ofs("output_8.bmp", std::ios::binary);
//    fmd::io::write_image_bmp_to_stream(bmp, ofs);
//    std::destroy_n(img_data, img_size);
//}

