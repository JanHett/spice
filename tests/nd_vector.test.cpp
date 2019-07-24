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

TEST(nd_vector, destructor_owner) {}
TEST(nd_vector, destructor_non_owner) {}

TEST(nd_vector, dimensions) {}
TEST(nd_vector, shape) {}
TEST(nd_vector, size) {}

TEST(nd_vector, copy_assignment_owner) {}
TEST(nd_vector, move_assignment_owner) {}

TEST(nd_vector, copy_assignment_non_owner) {}
TEST(nd_vector, move_assignment_non_owner) {}

TEST(nd_vector, operator_subscript_n_dim) {}
TEST(nd_vector, operator_subscript_n_dim_const) {}

TEST(nd_vector, operator_subscript_one_dim) {}
TEST(nd_vector, operator_subscript_one_dim_const) {}

TEST(nd_vector, operator_call_intermediate_dim) {}
TEST(nd_vector, operator_call_intermediate_dim_const) {}

TEST(nd_vector, operator_call_lowest_dim) {}
TEST(nd_vector, operator_call_lowest_dim_const) {}
