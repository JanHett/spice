#include <gtest/gtest.h>

#include "../src/blur.hpp"
#include "../src/statistics.hpp"
#include "../src/print.hpp"

using namespace spice;

TEST(fast_gaussian, normal_call) {
    // GTEST_SKIP();
    auto boat = load_image<float>("../data/testing/boat.jpg");

    print::image(boat, 6);
    auto b_boat = blur::fast_gaussian(boat, 10);
    print::image(b_boat, 6);

    // for (size_t x = 0; x < boat.width(); x += 10) {
    //     for (size_t y = 0; y < boat.height(); y += 10) {
    //         std::cout << "{" <<
    //         boat(x, y, 0) << "|" <<
    //         boat(x, y, 1) << "|" <<
    //         boat(x, y, 2) << "}";
    //     }
    //     std::cout << "---\n";
    // }
}

TEST(fast_gaussian, radius_larger_than_image) {
    GTEST_SKIP();
}
