#include <gtest/gtest.h>

#include <spice/composite.hpp>

#include "test_utils.hpp"

#include <spice/print.hpp>

using namespace spice;

TEST(interpolation, nearest_neighbor) {
    auto img = make_checkerboard<float>(2,1);

    color<float> white({3}, 1.f);
    color<float> black({3}, 0.f);

    interpolation::nearest_neighbor<float> interpolate(img);

    EXPECT_EQ(white, interpolate(0.9, 0.9));
    EXPECT_EQ(black, interpolate(0.9, 1.9));

    // make sure overflow is handled correctly
    EXPECT_EQ(black, interpolate(42.47, 47.42));
    EXPECT_EQ(black, interpolate(-42.47, -47.42));
}

TEST(interpolation, nearest_neighbor_round) {
    auto img = make_checkerboard<float>(2,1);

    color<float> white({3}, 1.f);
    color<float> black({3}, 0.f);

    interpolation::nearest_neighbor_round<float> interpolate(img);

    EXPECT_EQ(white, interpolate(0.42, 0.47));
    EXPECT_EQ(black, interpolate(0.123, 1.3));

    // make sure overflow is handled correctly
    EXPECT_EQ(black, interpolate(42.47, 47.42));
    EXPECT_EQ(black, interpolate(-42.47, -47.42));
}

TEST(interpolation, bilinear) {
    auto img = make_checkerboard<float>(3,3);

    // print::image(img);

    color<float> black({3}, 0.f);
    color<float> grey_25({3}, 0.25f);
    color<float> grey_50({3}, 0.5f);
    color<float> grey_75({3}, 0.75f);
    color<float> white({3}, 1.f);

    interpolation::bilinear<float> interpolate(img);

    EXPECT_EQ(white, interpolate(0.f, 0.f));
    EXPECT_EQ(grey_75, interpolate(0.25, 0.f));
    EXPECT_EQ(grey_50, interpolate(0.5, 0.5));
    EXPECT_EQ(grey_25, interpolate(0.75, 0.f));
    EXPECT_EQ(black, interpolate(1.f, 0.f));

    // make sure overflow is handled correctly
    EXPECT_EQ(black, interpolate(42.47, 47.42));
    EXPECT_EQ(black, interpolate(-42.47, -47.42));
}

TEST(interpolation, bicubic) { GTEST_SKIP(); }

TEST(blend_function, overlay_no_alpha) {
    color<float> fg({3}, 0.47);
    color<float> bg({3}, 0.123);

    blend_function::overlay<float> blend;

    auto blended = blend(bg, fg);

    EXPECT_FLOAT_EQ(blended[0], 0.47f);
    EXPECT_FLOAT_EQ(blended[1], 0.47f);
    EXPECT_FLOAT_EQ(blended[2], 0.47f);
}

TEST(blend_function, overlay_with_alpha) {
    color<float> fg({4}, 0.6);
    color<float> bg({4}, 0.1);

    // fully opaque background
    bg[2] = 1.f;

    // "red"-ish foreground
    fg[1] = 0.2;
    fg[3] = 0.3;

    // alpha at index 2 - odd, but let's test it to be sure it works
    blend_function::overlay<float> blend(2);

    auto blended = blend(bg, fg);

    EXPECT_FLOAT_EQ(blended[0], 0.64f);
    EXPECT_FLOAT_EQ(blended[1], 0.24f);
    EXPECT_FLOAT_EQ(blended[2], 1.f);
    EXPECT_FLOAT_EQ(blended[3], 0.34f);
}

TEST(merge, merge_w_translate) {
    image<float> a(50, 50, {"R", "G", "B"});
    image<float> b(50, 50, {"R", "G", "B"}, NO_ALPHA,
        image<float>::intensity_range.max);

    transform_2d tx(10, 20, 0, 1, 1);

    merge<float, interpolation::bilinear<float>>(a, b, tx);

    // write_image("../data/testing/translate.jpg", a);

    color<float> black({3}, 0.f);
    color<float> white({3}, 1.f);

    EXPECT_EQ(black, a( 0, 0));
    EXPECT_EQ(black, a( 9, 19));
    EXPECT_EQ(white, a(10, 20));
}

TEST(merge, merge_w_rotate) {
    image<float> a(50, 50, {"R", "G", "B"});
    image<float> b(50, 50, {"R", "G", "B"}, NO_ALPHA,
        image<float>::intensity_range.max);

    transform_2d tx(0, 0, 47, 1, 1);

    merge<float, interpolation::bilinear<float>>(a, b, tx);

    // write_image("../data/testing/rotate.jpg", a);

    color<float> black({3}, 0.f);
    color<float> white({3}, 1.f);

    EXPECT_EQ(white, a( 0,  0));
    EXPECT_EQ(black, a( 2,  0));
    EXPECT_EQ(black, a(49, 49));
    EXPECT_EQ(white, a( 0, 49));
}

TEST(merge, merge_w_scale) {
    image<float> a(50, 50, {"R", "G", "B"});
    image<float> b(50, 50, {"R", "G", "B"}, NO_ALPHA,
        image<float>::intensity_range.max);

    transform_2d tx(0, 0, 0, 2, 0.5);

    merge<float, interpolation::bilinear<float>>(a, b, tx);

    // write_image("../data/testing/scale.jpg", a);

    color<float> black({3}, 0.f);
    color<float> white({3}, 1.f);

    EXPECT_EQ(white, a( 0,  0));
    EXPECT_EQ(white, a( 1,  0));
    EXPECT_EQ(white, a(49,  0));
    EXPECT_EQ(black, a( 0, 25));
    EXPECT_EQ(black, a(49, 25));
    EXPECT_EQ(black, a(49, 49));
}
