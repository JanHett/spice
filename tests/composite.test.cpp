#include <gtest/gtest.h>

#include <spice/composite.hpp>

#include "test_utils.hpp"

using namespace spice;

TEST(interpolation, nearest_neighbor_floor) {
    auto img = make_checkerboard<float>(2,1);

    color<float> white({3}, 1.f);
    color<float> black({3}, 0.f);

    EXPECT_EQ(white, interpolation::nearest_neighbor_floor(img, {0.9, 0.9}));
    EXPECT_EQ(black, interpolation::nearest_neighbor_floor(img, {0.9, 1.9}));
}

TEST(interpolation, nearest_neighbor_round) {
    auto img = make_checkerboard<float>(2,1);

    color<float> white({3}, 1.f);
    color<float> black({3}, 0.f);

    EXPECT_EQ(white, interpolation::nearest_neighbor_round(img, {0.42, 0.47}));
    EXPECT_EQ(black, interpolation::nearest_neighbor_round(img, {0.123, 1.3}));
}

TEST(interpolation, nearest_neighbor) {
    auto img = make_checkerboard<float>(2,1);

    color<float> white({3}, 1.f);
    color<float> black({3}, 0.f);

    EXPECT_EQ(white, interpolation::nearest_neighbor(img, {0.9, 0.9}));
    EXPECT_EQ(black, interpolation::nearest_neighbor(img, {0.9, 1.9}));
}

TEST(interpolation, bilinear) { GTEST_SKIP(); }

TEST(interpolation, bicubic) { GTEST_SKIP(); }

TEST(merge, merge) { GTEST_SKIP(); }
