#include <gtest/gtest.h>

#include <spice/matrix.hpp>

using namespace spice;

TEST(matrix_internal, matmul_internal) {
    // common_dim = 2;
    // a_rows = 4;
    // b_columns = 3;

    int a_data[] = {
        1, 9782, 13, 398,
        0,    1,  0,  42} ;
    int b_data[] = {
        12, 39487,
        23,    146,
         0,      1 };

    auto c = matmul_internal<int>(a_data, b_data, 2, 4, 3);

    EXPECT_EQ(12, c.size());

    EXPECT_EQ(     12, c.data()[ 0]);
    EXPECT_EQ( 156871, c.data()[ 1]);
    EXPECT_EQ(    156, c.data()[ 2]);
    EXPECT_EQ(1663230, c.data()[ 3]);
    EXPECT_EQ(     23, c.data()[ 4]);
    EXPECT_EQ( 225132, c.data()[ 5]);
    EXPECT_EQ(    299, c.data()[ 6]);
    EXPECT_EQ(  15286, c.data()[ 7]);
    EXPECT_EQ(      0, c.data()[ 8]);
    EXPECT_EQ(      1, c.data()[ 9]);
    EXPECT_EQ(      0, c.data()[10]);
    EXPECT_EQ(     42, c.data()[11]);
}
