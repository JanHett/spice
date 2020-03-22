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

TEST(matrix, default_constructor) {
    matrix<float> m;

    EXPECT_EQ(0, m.columns());
    EXPECT_EQ(0, m.rows());
}

TEST(matrix, size_constructor) {
    matrix<float> m1(3, 4);

    EXPECT_EQ(3, m1.columns());
    EXPECT_EQ(4, m1.rows());

    // 1 0 0
    // 0 1 0
    // 0 0 1
    // 0 0 0

    EXPECT_EQ(1, m1.data()[ 0]);
    EXPECT_EQ(0, m1.data()[ 1]);
    EXPECT_EQ(0, m1.data()[ 2]);
    EXPECT_EQ(0, m1.data()[ 3]);
    EXPECT_EQ(0, m1.data()[ 4]);
    EXPECT_EQ(1, m1.data()[ 5]);
    EXPECT_EQ(0, m1.data()[ 6]);
    EXPECT_EQ(0, m1.data()[ 7]);
    EXPECT_EQ(0, m1.data()[ 8]);
    EXPECT_EQ(0, m1.data()[ 9]);
    EXPECT_EQ(1, m1.data()[10]);
    EXPECT_EQ(0, m1.data()[11]);

    matrix<float> m2(5, 3);

    EXPECT_EQ(5, m2.columns());
    EXPECT_EQ(3, m2.rows());

    // 1 0 0 0 0
    // 0 1 0 0 0
    // 0 0 1 0 0

    EXPECT_EQ(1, m2.data()[ 0]);
    EXPECT_EQ(0, m2.data()[ 1]);
    EXPECT_EQ(0, m2.data()[ 2]);
    EXPECT_EQ(0, m2.data()[ 3]);
    EXPECT_EQ(1, m2.data()[ 4]);
    EXPECT_EQ(0, m2.data()[ 5]);
    EXPECT_EQ(0, m2.data()[ 6]);
    EXPECT_EQ(0, m2.data()[ 7]);
    EXPECT_EQ(1, m2.data()[ 8]);
    EXPECT_EQ(0, m2.data()[ 9]);
    EXPECT_EQ(0, m2.data()[10]);
    EXPECT_EQ(0, m2.data()[11]);
    EXPECT_EQ(0, m2.data()[12]);
    EXPECT_EQ(0, m2.data()[13]);
    EXPECT_EQ(0, m2.data()[14]);
}

TEST (matrix, invert) {
    matrix<float> m(3, 3);

    // starting matrix
    // 3 0  2
    // 2 0 -2
    // 0 1  1
    m.data()[0] =  3;
    m.data()[1] =  2;
    m.data()[2] =  0;
    m.data()[3] =  0;
    m.data()[4] =  0;
    m.data()[5] =  1;
    m.data()[6] =  2;
    m.data()[7] = -2;
    m.data()[8] =  1;

    auto m_inv = invert(m);

    // expectation
    //  0.2  0.2 0
    // -0.2  0.3 1
    //  0.2 -0.3 0
    EXPECT_FLOAT_EQ( 0.2, m_inv.data()[0]);
    EXPECT_FLOAT_EQ(-0.2, m_inv.data()[1]);
    EXPECT_FLOAT_EQ( 0.2, m_inv.data()[2]);
    EXPECT_FLOAT_EQ( 0.2, m_inv.data()[3]);
    EXPECT_FLOAT_EQ( 0.3, m_inv.data()[4]);
    EXPECT_FLOAT_EQ(-0.3, m_inv.data()[5]);
    EXPECT_FLOAT_EQ( 0  , m_inv.data()[6]);
    EXPECT_FLOAT_EQ( 1  , m_inv.data()[7]);
    EXPECT_FLOAT_EQ( 0  , m_inv.data()[8]);
}
