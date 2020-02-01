#include <gtest/gtest.h>

#include <spice/composite.hpp>

#include "test_utils.hpp"

using namespace spice;

TEST(interpolation, nearest_neighbor_floor) {
    auto img = make_checkerboard<float>(2,1);

    color<float> white({3}, 1.f);
    color<float> black({3}, 0.f);

    interpolation::nearest_neighbor_floor<float> interpolate(img);

    EXPECT_EQ(white, interpolate(0.9, 0.9));
    EXPECT_EQ(black, interpolate(0.9, 1.9));
}

TEST(interpolation, nearest_neighbor_round) {
    auto img = make_checkerboard<float>(2,1);

    color<float> white({3}, 1.f);
    color<float> black({3}, 0.f);

    interpolation::nearest_neighbor_round<float> interpolate(img);

    EXPECT_EQ(white, interpolate(0.42, 0.47));
    EXPECT_EQ(black, interpolate(0.123, 1.3));
}

TEST(interpolation, nearest_neighbor) {
    auto img = make_checkerboard<float>(2,1);

    color<float> white({3}, 1.f);
    color<float> black({3}, 0.f);

    interpolation::nearest_neighbor<float> interpolate(img);

    EXPECT_EQ(white, interpolate(0.9, 0.9));
    EXPECT_EQ(black, interpolate(0.9, 1.9));
}

TEST(interpolation, bilinear) { GTEST_SKIP(); }

TEST(interpolation, bicubic) { GTEST_SKIP(); }

TEST(merge, merge) {
    auto a = load_image<float>("../data/testing/boat.jpg");
    auto b = a;

    transform_2d tx(42, 47, 47, 0.42, 0.47);

    merge(a, b, tx);

    write_image("../data/testing/boat_tx.jpg", a);

    //
    // abstract art...
    //

    image<float> bg(512, 512, {"R", "G", "B"});
    image<float> square(128, 128, {"R", "G", "B"});
    color<float> white({3}, 1.f);

    // paint bg
    for (size_t x = 0; x < bg.width(); ++x) {
        for (size_t y = 0; y < bg.height(); ++y) {
            for (size_t chan = 0; chan < bg.channels(); ++chan) {
                bg(x, y, chan) = (
                    std::sinf((static_cast<float>(x) / bg.width()) * 10 * chan) +
                    1 - std::cosf((static_cast<float>(y) / bg.height()) * 10 * chan)
                    ) / 2;
            }
        }
    }

    // paint square colours
    for (size_t x = 0; x < square.width(); ++x) {
        for (size_t y = 0; y < square.height(); ++y) {
            for (size_t chan = 0; chan < square.channels(); ++chan) {
                square(x, y, chan) = (
                    1 - std::cosf((static_cast<float>(x) / square.width()) * chan) +
                    std::sinf((static_cast<float>(y) / square.height()) * chan)
                    ) / 2 * chan;
            }
        }
    }

    // paint borders of square white
    for (size_t x = 0; x < square.width(); ++x) {
        square(x, 0) = white;
        square(x, 1) = white;
        square(x, 126) = white;
        square(x, 127) = white;
    }
    for (size_t y = 0; y < square.height(); ++y) {
        square(0, y) = white;
        square(1, y) = white;
        square(126, y) = white;
        square(127, y) = white;
    }

    transform_2d abstract_tx(165, 256, 45, 1, 1);
    merge(bg, square, abstract_tx);
    write_image("../data/testing/abstract_generated.jpg", bg);
}
