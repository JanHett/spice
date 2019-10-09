#include <gtest/gtest.h>

#include "../src/blur.hpp"
#include "../src/statistics.hpp"
#include "../src/print.hpp"

using namespace spice;

TEST(fast_gaussian, normal_call) {
    // GTEST_SKIP();
    auto boat = load_image<float>("../data/testing/boat.jpg");

    // print::image(boat, 6);
    auto b_boat = blur::fast_gaussian(boat, 10);
    // print::image(b_boat, 6);
    // write_image("../data/testing/boat_blurred.jpg", b_boat);

    EXPECT_EQ(boat.width(), b_boat.width());
    EXPECT_EQ(boat.height(), b_boat.height());
    // TODO: check if the image is actually blurred in a realistic way
    // (e.g. subtract from model image and check if difference is small enough).
}

TEST(fast_gaussian, radius_larger_than_image) {
    GTEST_SKIP();
}
