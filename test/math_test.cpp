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

    ffm::basic_matrix<float, 4, 4> x(
        1,2,3,4,
        4,3,2,1,
        1,4,3,2,
        2,1,4,3
    );

    print_matrix(ffm::inv(x));
}
