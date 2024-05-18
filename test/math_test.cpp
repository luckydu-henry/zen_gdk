#include <print>
#include <vector>
#include <iostream>

#include "fmath/complex.hpp"
#include "fmath/tensor_view.hpp"

int main(int argc, char* argv[]) {
    using std::print;
    namespace ranges = std::ranges;
    namespace ffm = force::math;

    using namespace ffm::binaron_literals;
    // Generalized complex number (binaron and quaternion)
    // For rotation (lerp is still waiting to be implemented) and complex analyzes.
    // ...
    // Literal suffix (orthographic base form)
    // Basic function (normal and exponent e.g. polar form)

    // Matrix view for image (audio) process.


    //auto load_bmp = [](const char* path, std::pmr::vector<unsigned char>& buf)->ffm::matrix_view<unsigned char> {
    //    // Open file
    //    // ...
    //    // Parse width height bpp
    //    buf.resize(width * height * bpp);
    //    // Read data to buf
    //    auto view = ffm::matrix_view<unsigned char>(buf.data(), bpp, width, width * bpp, height);
    //    // If layout requires to flip then
    //    return view.reverse(ffm::access::vertical);
    //};


    std::pmr::vector<float> v{
        255,255,255, 0,0,0,
        1,1,1, 255,255,255
    };

    ffm::matrix_view<float> view(v.data(), 3, 2, 3 * 2, 2);

    view = view.transpose();

    ffm::for_each(view, [](auto& v) { print("{},", v); }, [] { print("\n"); });
}