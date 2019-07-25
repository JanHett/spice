#include <gtest/gtest.h>

#include "../src/nd_vector.hpp"

using namespace spice;

TEST(nd_vector, default_constructor) {
    nd_vector<3, float> ndv;

    EXPECT_EQ(3, ndv.dimensions());
    for (auto const & dim_s : ndv.shape())
        EXPECT_EQ(0, dim_s);

    EXPECT_EQ(nullptr, ndv.data());
}
TEST(nd_vector, copy_constructor) {
    // create data array
    float * non_owned_data = new float[10];

    // populate them
    for (int i = 0; i < 10; ++i)
        non_owned_data[i] = i;

    // create non-owning nd_vector and make an owning copy of it
    nd_vector<2, float, false> non_owning_ndv(non_owned_data, {2, 5});
    nd_vector<2, float> copied_owning_ndv(non_owning_ndv);

    // check if shape is correct
    EXPECT_EQ(copied_owning_ndv.shape().size(), 2);
    EXPECT_EQ(copied_owning_ndv.shape()[0], 2);
    EXPECT_EQ(copied_owning_ndv.shape()[1], 5);

    // check if the contained data is correct
    for (int i = 0; i < 10; ++i)
        EXPECT_EQ(copied_owning_ndv.data()[i], i);

    // clean up
    delete[] non_owned_data;
}
TEST(nd_vector, move_constructor) {
    // create data array
    float * owned_data = new float[10];

    // populate them
    for (int i = 0; i < 10; ++i)
        owned_data[i] = i;

    // create owning nd_vector and move it into another right away
    nd_vector<2, float> movable_ndv(owned_data, {2, 5});
    nd_vector<2, float> moved_owning_ndv(std::move(movable_ndv));

    // check if shape is correct
    EXPECT_EQ(moved_owning_ndv.shape().size(), 2);
    EXPECT_EQ(moved_owning_ndv.shape()[0], 2);
    EXPECT_EQ(moved_owning_ndv.shape()[1], 5);

    // check if the contained data is correct
    for (int i = 0; i < 10; ++i)
        EXPECT_EQ(moved_owning_ndv.data()[i], i);
}
TEST(nd_vector, empty_constructor) {
    nd_vector<3, float> ndv({4,5,3});

    EXPECT_EQ(ndv.shape().size(), 3);
    EXPECT_EQ(ndv.shape()[0], 4);
    EXPECT_EQ(ndv.shape()[1], 5);
    EXPECT_EQ(ndv.shape()[2], 3);

    EXPECT_EQ(ndv.size(), 60);
    for (int i = 0; i < ndv.size(); ++i)
        EXPECT_EQ(ndv.data()[i], 0.f);
}
TEST(nd_vector, data_constructor) {
    // create data arrays
    float * non_owned_data = new float[10];
    float * owned_data = new float[20];

    // populate them
    for (int i = 0; i < 10; ++i)
        non_owned_data[i] = i;
    for (int i = 0; i < 20; ++i)
        owned_data[i] = i;

    // create owning and non-owning nd_vectors
    nd_vector<2, float, false> non_owning_ndv(non_owned_data, {2, 5});
    nd_vector<2, float> owning_ndv(owned_data, {4, 5});

    // check if shape is correct
    EXPECT_EQ(non_owning_ndv.shape().size(), 2);
    EXPECT_EQ(non_owning_ndv.shape()[0], 2);
    EXPECT_EQ(non_owning_ndv.shape()[1], 5);

    EXPECT_EQ(owning_ndv.shape().size(), 2);
    EXPECT_EQ(owning_ndv.shape()[0], 4);
    EXPECT_EQ(owning_ndv.shape()[1], 5);

    // check if the contained data is correct
    for (int i = 0; i < 10; ++i)
        EXPECT_EQ(non_owning_ndv.data()[i], i);
    for (int i = 0; i < 20; ++i)
        EXPECT_EQ(owning_ndv.data()[i], i);

    // clean up
    delete[] non_owned_data;
}

TEST(nd_vector, dimensions) {
    nd_vector<42, float> ndv;

    static_assert(ndv.dimensions() == 42,
        "nd_vector::dimensions returns incorrect value.");
}
TEST(nd_vector, shape) {
    nd_vector<3, float> ndv({1,2,3});

    EXPECT_EQ(ndv.shape(), (std::array<size_t, 3>{1,2,3}));
}
TEST(nd_vector, size) {
    nd_vector<3, float> ndv({1,2,3});

    EXPECT_EQ(ndv.size(), 6);
}

TEST(nd_vector, copy_assignment_owner) { GTEST_SKIP(); }
TEST(nd_vector, move_assignment_owner) { GTEST_SKIP(); }

TEST(nd_vector, copy_assignment_non_owner) { GTEST_SKIP(); }
TEST(nd_vector, move_assignment_non_owner) { GTEST_SKIP(); }

TEST(nd_vector, copy_assignment_scalar_owner) { GTEST_SKIP(); }
TEST(nd_vector, move_assignment_scalar_owner) { GTEST_SKIP(); }

TEST(nd_vector, copy_assignment_scalar_non_owner) { GTEST_SKIP(); }
TEST(nd_vector, move_assignment_scalar_non_owner) { GTEST_SKIP(); }

TEST(nd_vector, operator_subscript_n_dim) {
    float data1[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    nd_vector<2, float, false> ndv1(data1, {2, 5});

    EXPECT_EQ(ndv1[0], (nd_vector<1, float, false>(data1, {5})));

    float data2[] = {10, 11, 12, 13, 14};
    ndv1[0] = nd_vector<1, float, false>(data2, {5});
    EXPECT_EQ(ndv1[0], (nd_vector<1, float, false>(data2, {5})));
}
TEST(nd_vector, operator_subscript_n_dim_const) {
    float data1[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    const nd_vector<2, float, false> ndv1(data1, {2, 5});

    EXPECT_EQ(ndv1[0], (nd_vector<1, float, false>(data1, {5})));
    static_assert(std::is_const<decltype(ndv1[0])>::value,
        "nd_vector::operator[] does not preserve const-ness.");
}

TEST(nd_vector, operator_subscript_one_dim) { GTEST_SKIP(); }
TEST(nd_vector, operator_subscript_one_dim_const) { GTEST_SKIP(); }

TEST(nd_vector, operator_call_intermediate_dim) { GTEST_SKIP(); }
TEST(nd_vector, operator_call_intermediate_dim_const) { GTEST_SKIP(); }

TEST(nd_vector, operator_call_lowest_dim) { GTEST_SKIP(); }
TEST(nd_vector, operator_call_lowest_dim_const) { GTEST_SKIP(); }

TEST(nd_vector, at_intermediate_dim) { GTEST_SKIP(); }
TEST(nd_vector, at_intermediate_dim_const) { GTEST_SKIP(); }

TEST(nd_vector, at_lowest_dim) { GTEST_SKIP(); }
TEST(nd_vector, at_lowest_dim_const) { GTEST_SKIP(); }

TEST(nd_vector, operator_equals) {
    float data1[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    nd_vector<2, float, false> ndv1(data1, {2, 5});
    nd_vector<2, float, false> ndv2(data1, {2, 5});

    EXPECT_TRUE(ndv1 == ndv2);

    float data2[] = {0, 1, 2, 3, 42, 5, 6, 7, 8, 9};
    nd_vector<2, float, false> ndv3(data2, {2, 5});
    nd_vector<2, float, false> ndv4(data2, {5, 2});

    EXPECT_FALSE(ndv1 == ndv3);
    EXPECT_FALSE(ndv4 == ndv3);
}
TEST(nd_vector, operator_not_equals) {
    float data1[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    nd_vector<2, float, false> ndv1(data1, {2, 5});
    nd_vector<2, float, false> ndv2(data1, {2, 5});

    EXPECT_FALSE(ndv1 != ndv2);

    float data2[] = {0, 1, 2, 3, 42, 5, 6, 7, 8, 9};
    nd_vector<2, float, false> ndv3(data2, {2, 5});
    nd_vector<2, float, false> ndv4(data2, {5, 2});

    EXPECT_TRUE(ndv1 != ndv3);
    EXPECT_TRUE(ndv4 != ndv3);
}

TEST(nd_vector, operator_plus_equals) { GTEST_SKIP(); }
TEST(nd_vector, operator_plus) { GTEST_SKIP(); }

TEST(nd_vector, operator_minus_equals) { GTEST_SKIP(); }
TEST(nd_vector, operator_minus) { GTEST_SKIP(); }

TEST(nd_vector, operator_multiply_equals) { GTEST_SKIP(); }
TEST(nd_vector, operator_multiply) { GTEST_SKIP(); }

TEST(nd_vector, operator_divide_equals) { GTEST_SKIP(); }
TEST(nd_vector, operator_divide) { GTEST_SKIP(); }

TEST(nd_vector, operator_plus_equals_scalar) { GTEST_SKIP(); }
TEST(nd_vector, operator_plus_scalar) { GTEST_SKIP(); }

TEST(nd_vector, operator_minus_equals_scalar) { GTEST_SKIP(); }
TEST(nd_vector, operator_minus_scalar) { GTEST_SKIP(); }

TEST(nd_vector, operator_multiply_equals_scalar) { GTEST_SKIP(); }
TEST(nd_vector, operator_multiply_scalar) { GTEST_SKIP(); }

TEST(nd_vector, operator_divide_equals_scalar) { GTEST_SKIP(); }
TEST(nd_vector, operator_divide_scalar) { GTEST_SKIP(); }
