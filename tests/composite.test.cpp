#include <gtest/gtest.h>

#include <spice/composite.hpp>

using namespace spice;

TEST(transform_2d, constructor) {
    transform_2d tx;

    EXPECT_EQ(1, tx.data()[0]);
    EXPECT_EQ(0, tx.data()[1]);
    EXPECT_EQ(1, tx.data()[2]);
    EXPECT_EQ(0, tx.data()[3]);
}
