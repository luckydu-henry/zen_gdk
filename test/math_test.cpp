#include <print>
#include <vector>

#include "fmath/container/complex.hpp"
#include "fmath/algorithm/complex.hpp"

int main(int argc, char* argv[]) {

    using std::print;
    namespace ranges = std::ranges;
    namespace ffm = force::math;

    
    std::vector<float> vec = {
        1,1,1,  2,2,2,
        3,3,3,  4,4,4
    };

    ffm::matrix_view<float> v(vec.data(), 3, 2, 3 * 2, 2);

    auto r = ffm::reverse(v, ffm::sequences::seq_y);

    for (auto i = r.begin(); i != r.end(); ++i) {
        for (auto j = i.begin(); j != i.end(); ++j) {
            print("[{},{},{}],", j[0], j[1], j[2]);
        }
        print("\n");
    }
}
