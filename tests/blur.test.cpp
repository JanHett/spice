#include <gtest/gtest.h>

#include "../src/blur.hpp"
#include "../src/statistics.hpp"
#include "../src/print.hpp"

using namespace spice;

TEST(fast_gaussian, normal_call) {
    auto boat = load_image<float>("../data/testing/boat.jpg");

    blur::fast_gaussian(boat, 3);

    print::image(boat, 9);
}

TEST(fast_gaussian, radius_larger_than_image) {

}
