#include <gtest/gtest.h>

#include <spice/transform.hpp>

using namespace spice;

TEST(transform_2d, default_constructor) {
    transform_2d tx;

    // should be the neutral transformation matrix:
    // 1 0 0
    // 0 1 0
    // 0 0 1
    EXPECT_EQ(1, tx.data()[0]);
    EXPECT_EQ(0, tx.data()[1]);
    EXPECT_EQ(0, tx.data()[2]);
    EXPECT_EQ(0, tx.data()[3]);
    EXPECT_EQ(1, tx.data()[4]);
    EXPECT_EQ(0, tx.data()[5]);
    EXPECT_EQ(0, tx.data()[1]);
    EXPECT_EQ(0, tx.data()[1]);
    EXPECT_EQ(1, tx.data()[8]);
}

TEST(transform_2d, fully_specified_constructor) {
    // GTEST_SKIP();
    transform_2d tx(
        42, 47,
        123,
        420, 0.42);

    EXPECT_EQ((std::pair{42.f, 47.f}), tx.translate());
    EXPECT_EQ(123, tx.rotate());
    auto scale = tx.scale();
    EXPECT_FLOAT_EQ(420.f, scale.first);
    EXPECT_FLOAT_EQ(0.42f, scale.second);

    transform_2d tx2(
        0, 0,
        123,
        1, 1);
    EXPECT_EQ(123, tx2.rotate());

    transform_2d tx3(
        0, 0,
        0,
        1, 1);
    EXPECT_EQ(0, tx3.rotate());
}

TEST(transform_2d, translate) { GTEST_SKIP(); }

TEST(transform_2d, get_translate) { GTEST_SKIP(); }

TEST(transform_2d, rotate) { GTEST_SKIP(); }

TEST(transform_2d, get_rotate) { GTEST_SKIP(); }

TEST(transform_2d, scale) { GTEST_SKIP(); }

TEST(transform_2d, get_scale) { GTEST_SKIP(); }

TEST(transform_2d, create_translate) { GTEST_SKIP(); }

TEST(transform_2d, create_rotate) { GTEST_SKIP(); }

TEST(transform_2d, create_scale) { GTEST_SKIP(); }
