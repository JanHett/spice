#include <gtest/gtest.h>

#include <spice/noise.hpp>
#include <spice/statistics.hpp>
#include <spice/print.hpp>

using namespace spice;

TEST(noise, salt_and_pepper) {
    image<float> img(10, 10, {"R", "G", "B"}, DISABLE_ALPHA_DEDUCTION, 0.5);

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
    // auto boat_fl = load_image<float>("../data/testing/boat.jpg");
    // noise::uniform(boat_fl, -0.25f, 0.25f);
    // write_image("../data/testing/boat_uniform_float.jpg", boat_fl, OIIO::TypeDesc::UINT8);

    // auto boat_int = load_image<uint8_t>("../data/testing/boat.jpg");
    // noise::uniform(boat_int, uint8_t(0), uint8_t(64));
    // write_image("../data/testing/boat_uniform_int.jpg", boat_int, OIIO::TypeDesc::UINT8);
}

TEST(noise, gaussian) {
    GTEST_SKIP();
    // auto boat = load_image<float>("../data/testing/boat.jpg");
    // noise::gaussian(boat, 0.05f, 0.15f);
    // write_image("../data/testing/boat_gaussian.jpg", boat, OIIO::TypeDesc::UINT8);
}
