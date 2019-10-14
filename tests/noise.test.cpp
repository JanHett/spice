#include <gtest/gtest.h>

#include "../src/noise.hpp"
#include "../src/statistics.hpp"
#include "../src/print.hpp"

using namespace spice;

TEST(noise, salt_and_pepper) {
    image<float> img(10, 10, {"R", "G", "B"}, 0.5);

    noise::salt_and_pepper(img, 0.5);

    auto hist = statistics::histogram(img, 10);

    // expect black sample to be non-zero
    EXPECT_LT(0, hist[0][0]);
    // expect white sample to be non-zero
    EXPECT_LT(0, hist[0].back());

    // expect there to be about equally as many black pixels as there are
    // white ones
    auto large = std::max(hist[0][0], hist[0].back());
    auto small = std::min(hist[0][0], hist[0].back());
    EXPECT_GT(11, large - small);

    // auto boat = load_image<float>("../data/testing/boat.jpg");
    // noise::salt_and_pepper(boat, 0.05);
    // write_image("../data/testing/boat_sp.jpg", boat, OIIO::TypeDesc::UINT8);
}

TEST(noise, uniform) {
    GTEST_SKIP();
}

TEST(noise, gaussian) {
    GTEST_SKIP();
    // auto boat = load_image<float>("../data/testing/boat.jpg");
    // noise::gaussian(boat, 0.05f, 0.15f);
    // write_image("../data/testing/boat_gaussian.jpg", boat, OIIO::TypeDesc::UINT8);
}
