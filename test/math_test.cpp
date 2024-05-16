#include <print>
#include <vector>
#include <iostream>

#include "fmath/complex.hpp"
#include "fmath/tensor_view.hpp"

void print_matrix(const force::math::basic_matrix<float, 4, 4>& mat) {
    for (auto i = mat.begin(); i != mat.end(); ++i) {
        for (auto j = i.begin(); j != i.end(); ++j) {
            std::print("{}, ", *j);
        }
        std::print("\n");
    }
}

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

    auto z = 1.F + 2.iF;


    auto v = z.dot(z);
    print("{}", v);


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


}