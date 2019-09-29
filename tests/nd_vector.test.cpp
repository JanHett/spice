#include <gtest/gtest.h>

#include "src/nd_vector.hpp"

using namespace spice;

TEST(nd_vector, default_constructor) {
    nd_vector_impl<3, float> ndv;

    EXPECT_EQ(3, ndv.dimensions());
    for (auto const & dim_s : ndv.shape())
        EXPECT_EQ(0, dim_s);

    EXPECT_EQ(nullptr, ndv.data().data());
}
TEST(nd_vector, copy_constructor) {
    // create data array
    float * non_owned_data = new float[10];

    // populate them
    for (int i = 0; i < 10; ++i)
        non_owned_data[i] = i;

    // create non-owning nd_vector_impl and make an owning copy of it
    nd_vector_impl<2, float, false> non_owning_ndv(non_owned_data, {2, 5});
    nd_vector_impl<2, float, false> copied_non_owning_ndv(non_owning_ndv);
    nd_vector_impl<2, float> copied_owning_ndv(non_owning_ndv);

    // check if shape is correct
    EXPECT_EQ(copied_owning_ndv.shape().size(), 2);
    EXPECT_EQ(copied_owning_ndv.shape()[0], 2);
    EXPECT_EQ(copied_owning_ndv.shape()[1], 5);

    EXPECT_EQ(copied_non_owning_ndv.shape().size(), 2);
    EXPECT_EQ(copied_non_owning_ndv.shape()[0], 2);
    EXPECT_EQ(copied_non_owning_ndv.shape()[1], 5);

    // check if the contained data is correct
    for (int i = 0; i < 10; ++i)
        EXPECT_EQ(copied_owning_ndv.data()[i], i);
    EXPECT_EQ(non_owning_ndv.data().data(),
        copied_non_owning_ndv.data().data());

    // clean up
    delete[] non_owned_data;
}
TEST(nd_vector, move_constructor) {
    // create data array
    float * owned_data = new float[10];

    // populate them
    for (int i = 0; i < 10; ++i)
        owned_data[i] = i;

    // create owning nd_vector_impl and move it into another right away
    nd_vector_impl<2, float> movable_ndv(owned_data, {2, 5});
    nd_vector_impl<2, float> moved_owning_ndv(std::move(movable_ndv));

    // check if shape is correct
    EXPECT_EQ(moved_owning_ndv.shape().size(), 2);
    EXPECT_EQ(moved_owning_ndv.shape()[0], 2);
    EXPECT_EQ(moved_owning_ndv.shape()[1], 5);

    // check if the contained data is correct
    for (int i = 0; i < 10; ++i)
        EXPECT_EQ(moved_owning_ndv.data()[i], i);
}
TEST(nd_vector, empty_constructor) {
    nd_vector_impl<3, float> ndv({4, 5, 3});

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
    nd_vector_impl<2, float, false> non_owning_ndv(non_owned_data, {2, 5});
    nd_vector_impl<2, float> owning_ndv(owned_data, {4, 5});

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
    nd_vector_impl<42, float> ndv;

    static_assert(ndv.dimensions() == 42,
        "nd_vector_impl::dimensions returns incorrect value.");
}
TEST(nd_vector, shape) {
    nd_vector_impl<3, float> ndv({1, 2, 3});

    EXPECT_EQ(ndv.shape(), (std::array<size_t, 3>{1,2,3}));
}
TEST(nd_vector, size) {
    nd_vector_impl<3, float> ndv({1, 2, 3});

    EXPECT_EQ(ndv.size(), 6);
}

TEST(nd_vector, copy_assignment_owner) {
    float * data_1d_1 = new float[5]{5, 6, 7, 8, 9};
    nd_vector_impl<1, float, true> ndv_1d_1(data_1d_1, {5});
    float * data_1d_2 = new float[5]{0, 1, 2, 3, 4};
    nd_vector_impl<1, float, true> ndv_1d_2(data_1d_2, {5});

    ndv_1d_1 = ndv_1d_2;

    EXPECT_EQ(0, ndv_1d_1.data()[0]);
    EXPECT_EQ(1, ndv_1d_1.data()[1]);
    EXPECT_EQ(2, ndv_1d_1.data()[2]);
    EXPECT_EQ(3, ndv_1d_1.data()[3]);
    EXPECT_EQ(4, ndv_1d_1.data()[4]);
    EXPECT_NE(ndv_1d_2.data().data(), ndv_1d_1.data().data());

   float * data1 = new float[10]{
       0, 1, 2, 3, 4,
       5, 6, 7, 8, 9};
   nd_vector_impl<2, float, true> ndv1(data1, {5, 2});

   float * data2 = new float[10]{
       10, 11, 12, 13, 14,
       15, 16, 17, 18, 19};
   nd_vector_impl<2, float, true> ndv2(data2, {5, 2});

   ndv1 = ndv2;

   EXPECT_EQ(10, ndv1.data()[0]);
   EXPECT_EQ(11, ndv1.data()[1]);
   EXPECT_EQ(12, ndv1.data()[2]);
   EXPECT_EQ(13, ndv1.data()[3]);
   EXPECT_EQ(14, ndv1.data()[4]);
   EXPECT_EQ(15, ndv1.data()[5]);
   EXPECT_EQ(16, ndv1.data()[6]);
   EXPECT_EQ(17, ndv1.data()[7]);
   EXPECT_EQ(18, ndv1.data()[8]);
   EXPECT_EQ(19, ndv1.data()[9]);
   EXPECT_NE(ndv2.data().data(), ndv1.data().data());
}
TEST(nd_vector, move_assignment_owner) { GTEST_SKIP(); }

TEST(nd_vector, copy_assignment_non_owner) {
    float data_1d_1[] = {5, 6, 7, 8, 9};
    nd_vector_impl<1, float, false> ndv_1d_1(data_1d_1, {5});
    float data_1d_2[] = {0, 1, 2, 3, 4};
    nd_vector_impl<1, float, false> ndv_1d_2(data_1d_2, {5});

    ndv_1d_1 = ndv_1d_2;

    EXPECT_EQ(0, ndv_1d_1.data()[0]);
    EXPECT_EQ(1, ndv_1d_1.data()[1]);
    EXPECT_EQ(2, ndv_1d_1.data()[2]);
    EXPECT_EQ(3, ndv_1d_1.data()[3]);
    EXPECT_EQ(4, ndv_1d_1.data()[4]);
    EXPECT_NE(ndv_1d_2.data().data(), ndv_1d_1.data().data());

   float data1[] = {
       0, 1, 2, 3, 4,
       5, 6, 7, 8, 9};
   nd_vector_impl<2, float, false> ndv1(data1, {5, 2});

   float data2[] = {
       10, 11, 12, 13, 14,
       15, 16, 17, 18, 19};
   nd_vector_impl<2, float, false> ndv2(data2, {5, 2});

   ndv1 = ndv2;

   EXPECT_EQ(10, ndv1.data()[0]);
   EXPECT_EQ(11, ndv1.data()[1]);
   EXPECT_EQ(12, ndv1.data()[2]);
   EXPECT_EQ(13, ndv1.data()[3]);
   EXPECT_EQ(14, ndv1.data()[4]);
   EXPECT_EQ(15, ndv1.data()[5]);
   EXPECT_EQ(16, ndv1.data()[6]);
   EXPECT_EQ(17, ndv1.data()[7]);
   EXPECT_EQ(18, ndv1.data()[8]);
   EXPECT_EQ(19, ndv1.data()[9]);
   EXPECT_NE(ndv2.data().data(), ndv1.data().data());
}
TEST(nd_vector, move_assignment_non_owner) { GTEST_SKIP(); }

TEST(nd_vector, copy_assignment_mixed) {
    float * data_1d_1 = new float[5]{5, 6, 7, 8, 9};
    nd_vector_impl<1, float, true> ndv_1d_1(data_1d_1, {5});
    float data_1d_2[] = {0, 1, 2, 3, 4};
    nd_vector_impl<1, float, false> ndv_1d_2(data_1d_2, {5});

    ndv_1d_1 = ndv_1d_2;

    EXPECT_EQ(0, ndv_1d_1.data()[0]);
    EXPECT_EQ(1, ndv_1d_1.data()[1]);
    EXPECT_EQ(2, ndv_1d_1.data()[2]);
    EXPECT_EQ(3, ndv_1d_1.data()[3]);
    EXPECT_EQ(4, ndv_1d_1.data()[4]);
    EXPECT_NE(ndv_1d_2.data().data(), ndv_1d_1.data().data());

   float data1[] = {
       0, 1, 2, 3, 4,
       5, 6, 7, 8, 9};
   nd_vector_impl<2, float, false> ndv1(data1, {5, 2});

   float * data2 = new float[10]{
       10, 11, 12, 13, 14,
       15, 16, 17, 18, 19};
   nd_vector_impl<2, float, true> ndv2(data2, {5, 2});

   ndv1 = ndv2;

   EXPECT_EQ(10, ndv1.data()[0]);
   EXPECT_EQ(11, ndv1.data()[1]);
   EXPECT_EQ(12, ndv1.data()[2]);
   EXPECT_EQ(13, ndv1.data()[3]);
   EXPECT_EQ(14, ndv1.data()[4]);
   EXPECT_EQ(15, ndv1.data()[5]);
   EXPECT_EQ(16, ndv1.data()[6]);
   EXPECT_EQ(17, ndv1.data()[7]);
   EXPECT_EQ(18, ndv1.data()[8]);
   EXPECT_EQ(19, ndv1.data()[9]);
   EXPECT_NE(ndv2.data().data(), ndv1.data().data());
}
TEST(nd_vector, move_assignment_mixed) { GTEST_SKIP(); }

TEST(nd_vector, copy_assignment_scalar_owner) {
    float * data = new float[6]{5, 6, 7, 8, 9, 0};
    nd_vector_impl<2, float, true> ndv(data, {3, 2});

    ndv = 42;

    for (int idx = 0; idx < 5; ++idx)
        EXPECT_EQ(42, ndv.data()[idx]);
}

TEST(nd_vector, copy_assignment_scalar_non_owner) {
    float data[] = {0, 1, 2, 3, 4, 5, 6, 7};
    nd_vector_impl<3, float, false> ndv(data, {2, 2, 2});

    ndv = 47;

    for (int idx = 0; idx < 5; ++idx)
        EXPECT_EQ(47, ndv.data()[idx]);
}

TEST(nd_vector, operator_subscript_n_dim) {
    float data1[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    nd_vector_impl<2, float, false> ndv1(data1, {2, 5});

    EXPECT_EQ(ndv1[0], (nd_vector_impl<1, float, false>(data1, {5})));

    // test assigning to returned non-owning nd_vector_impl
    float data2[] = {10, 11, 12, 13, 14};
    nd_vector_impl<1, float, false> new_ndv(data2, {5});
    ndv1[0] = new_ndv;
    EXPECT_EQ(ndv1[0], new_ndv);
}
TEST(nd_vector, operator_subscript_n_dim_const) {
    float data1[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    const nd_vector_impl<2, float, false> ndv1(data1, {2, 5});

    EXPECT_EQ(ndv1[0], (nd_vector_impl<1, float, false>(data1, {5})));
    static_assert(std::is_const<decltype(ndv1[0])>::value,
        "nd_vector_impl::operator[] does not preserve const-ness.");
}

TEST(nd_vector, operator_subscript_one_dim) { GTEST_SKIP(); }
TEST(nd_vector, operator_subscript_one_dim_const) { GTEST_SKIP(); }

TEST(nd_vector, operator_call_intermediate_dim) {
    float data1[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    nd_vector_impl<2, float, false> ndv1(data1, {2, 5});

    auto one_dim = ndv1(1);
    static_assert(std::is_same<
            decltype(one_dim),
            nd_vector_impl<1, float, false>
        >::value,
        "Intermediate-dimensional call operator does not return an nd-vector");

    EXPECT_EQ((std::array<size_t, 1>{5}), one_dim.shape());

    EXPECT_EQ(5, one_dim[0]);
    EXPECT_EQ(6, one_dim[1]);
    EXPECT_EQ(7, one_dim[2]);
    EXPECT_EQ(8, one_dim[3]);
    EXPECT_EQ(9, one_dim[4]);
}
TEST(nd_vector, operator_call_intermediate_dim_const) {
    float data1[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    const nd_vector_impl<2, float, false> ndv1(data1, {2, 5});

    auto const one_dim = ndv1(0);
    static_assert(std::is_same<
            decltype(one_dim),
            const nd_vector_impl<1, float, false>
        >::value,
        "Intermediate-dimensional call operator does not return an nd-vector");

    EXPECT_EQ((std::array<size_t, 1>{5}), one_dim.shape());

    EXPECT_EQ(0, one_dim(0));
    EXPECT_EQ(1, one_dim(1));
    EXPECT_EQ(2, one_dim(2));
    EXPECT_EQ(3, one_dim(3));
    EXPECT_EQ(4, one_dim(4));
}

TEST(nd_vector, operator_call_lowest_dim) {
    float data1[] = {
         0,  1,  2,  3,  4,
         5,  6,  7,  8,  9,
        10, 11, 12, 13, 14,
        15, 16, 17, 18, 19
    };
    nd_vector_impl<3, float, false> ndv1(data1, {2, 2, 5});

    EXPECT_EQ( 0, ndv1(0, 0, 0));
    EXPECT_EQ( 1, ndv1(0, 0, 1));
    EXPECT_EQ( 2, ndv1(0, 0, 2));
    EXPECT_EQ( 3, ndv1(0, 0, 3));
    EXPECT_EQ( 4, ndv1(0, 0, 4));
    EXPECT_EQ( 5, ndv1(0, 1, 0));
    EXPECT_EQ( 6, ndv1(0, 1, 1));
    EXPECT_EQ( 7, ndv1(0, 1, 2));
    EXPECT_EQ( 8, ndv1(0, 1, 3));
    EXPECT_EQ( 9, ndv1(0, 1, 4));
    EXPECT_EQ(10, ndv1(1, 0, 0));
    EXPECT_EQ(11, ndv1(1, 0, 1));
    EXPECT_EQ(12, ndv1(1, 0, 2));
    EXPECT_EQ(13, ndv1(1, 0, 3));
    EXPECT_EQ(14, ndv1(1, 0, 4));
    EXPECT_EQ(15, ndv1(1, 1, 0));
    EXPECT_EQ(16, ndv1(1, 1, 1));
    EXPECT_EQ(17, ndv1(1, 1, 2));
    EXPECT_EQ(18, ndv1(1, 1, 3));
    EXPECT_EQ(19, ndv1(1, 1, 4));
}
TEST(nd_vector, operator_call_lowest_dim_const) {
    float data1[] = {
         0,  1,  2,  3,  4,
         5,  6,  7,  8,  9,
        10, 11, 12, 13, 14,
        15, 16, 17, 18, 19
    };
    const nd_vector_impl<3, float, false> ndv1(data1, {2, 2, 5});

    EXPECT_EQ( 0, ndv1(0, 0, 0));
    EXPECT_EQ( 1, ndv1(0, 0, 1));
    EXPECT_EQ( 2, ndv1(0, 0, 2));
    EXPECT_EQ( 3, ndv1(0, 0, 3));
    EXPECT_EQ( 4, ndv1(0, 0, 4));
    EXPECT_EQ( 5, ndv1(0, 1, 0));
    EXPECT_EQ( 6, ndv1(0, 1, 1));
    EXPECT_EQ( 7, ndv1(0, 1, 2));
    EXPECT_EQ( 8, ndv1(0, 1, 3));
    EXPECT_EQ( 9, ndv1(0, 1, 4));
    EXPECT_EQ(10, ndv1(1, 0, 0));
    EXPECT_EQ(11, ndv1(1, 0, 1));
    EXPECT_EQ(12, ndv1(1, 0, 2));
    EXPECT_EQ(13, ndv1(1, 0, 3));
    EXPECT_EQ(14, ndv1(1, 0, 4));
    EXPECT_EQ(15, ndv1(1, 1, 0));
    EXPECT_EQ(16, ndv1(1, 1, 1));
    EXPECT_EQ(17, ndv1(1, 1, 2));
    EXPECT_EQ(18, ndv1(1, 1, 3));
    EXPECT_EQ(19, ndv1(1, 1, 4));
}

TEST(nd_vector, at_intermediate_dim) {
    float data1[] = {
         0,  1,  2,  3,  4,
         5,  6,  7,  8,  9,
        10, 11, 12, 13, 14,
        15, 16, 17, 18, 19
    };
    nd_vector_impl<3, float, false> ndv1(data1, {2, 2, 5});

    EXPECT_THROW(auto fail = ndv1.at(42), std::out_of_range);

    EXPECT_EQ(ndv1.at(0, 0), ndv1(0, 0));
    EXPECT_EQ(ndv1.at(0, 1), ndv1(0, 1));
    EXPECT_EQ(ndv1.at(1, 0), ndv1(1, 0));
    EXPECT_EQ(ndv1.at(1, 1), ndv1(1, 1));
}
TEST(nd_vector, at_intermediate_dim_const) {
    float data1[] = {
         0,  1,  2,  3,  4,
         5,  6,  7,  8,  9,
        10, 11, 12, 13, 14,
        15, 16, 17, 18, 19
    };
    const nd_vector_impl<3, float, false> ndv1(data1, {2, 2, 5});

    EXPECT_THROW(auto fail = ndv1.at(42), std::out_of_range);

    EXPECT_EQ(ndv1.at(0, 0), ndv1(0, 0));
    EXPECT_EQ(ndv1.at(0, 1), ndv1(0, 1));
    EXPECT_EQ(ndv1.at(1, 0), ndv1(1, 0));
    EXPECT_EQ(ndv1.at(1, 1), ndv1(1, 1));
}

TEST(nd_vector, at_lowest_dim) {
    float data1[] = {
         0,  1,  2,  3,  4,
         5,  6,  7,  8,  9,
        10, 11, 12, 13, 14,
        15, 16, 17, 18, 19
    };
    nd_vector_impl<3, float, false> ndv1(data1, {2, 2, 5});

    EXPECT_THROW(auto fail = ndv1.at(42), std::out_of_range);

    EXPECT_EQ(ndv1.at(0, 0, 0), ndv1(0, 0, 0));
    EXPECT_EQ(ndv1.at(0, 1, 1), ndv1(0, 1, 1));
    EXPECT_EQ(ndv1.at(1, 0, 2), ndv1(1, 0, 2));
    EXPECT_EQ(ndv1.at(1, 1, 4), ndv1(1, 1, 4));
}
TEST(nd_vector, at_lowest_dim_const) {
    float data1[] = {
         0,  1,  2,  3,  4,
         5,  6,  7,  8,  9,
        10, 11, 12, 13, 14,
        15, 16, 17, 18, 19
    };
    const nd_vector_impl<3, float, false> ndv1(data1, {2, 2, 5});

    EXPECT_THROW(auto fail = ndv1.at(42), std::out_of_range);

    EXPECT_EQ(ndv1.at(0, 0, 4), ndv1(0, 0, 4));
    EXPECT_EQ(ndv1.at(0, 1, 2), ndv1(0, 1, 2));
    EXPECT_EQ(ndv1.at(1, 0, 0), ndv1(1, 0, 0));
    EXPECT_EQ(ndv1.at(1, 1, 1), ndv1(1, 1, 1));
}

TEST(nd_vector, operator_equals) {
    float data1[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    nd_vector_impl<2, float, false> ndv1(data1, {2, 5});
    nd_vector_impl<2, float, false> ndv2(data1, {2, 5});

    EXPECT_TRUE(ndv1 == ndv2);

    float data2[] = {0, 1, 2, 3, 42, 5, 6, 7, 8, 9};
    nd_vector_impl<2, float, false> ndv3(data2, {2, 5});
    nd_vector_impl<2, float, false> ndv4(data2, {5, 2});

    EXPECT_FALSE(ndv1 == ndv3);
    EXPECT_FALSE(ndv4 == ndv3);
}
TEST(nd_vector, operator_not_equals) {
    float data1[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    nd_vector_impl<2, float, false> ndv1(data1, {2, 5});
    nd_vector_impl<2, float, false> ndv2(data1, {2, 5});

    EXPECT_FALSE(ndv1 != ndv2);

    float data2[] = {0, 1, 2, 3, 42, 5, 6, 7, 8, 9};
    nd_vector_impl<2, float, false> ndv3(data2, {2, 5});
    nd_vector_impl<2, float, false> ndv4(data2, {5, 2});

    EXPECT_TRUE(ndv1 != ndv3);
    EXPECT_TRUE(ndv4 != ndv3);
}

TEST(nd_vector, operator_plus_equals) {
    float data1[] = {
         0,  1,  2,  3,
         5,  6,  7,  8,
        10, 11, 12, 13,
        15, 16, 17, 18
    };
    nd_vector_impl<3, float, false> ndv1(data1, {2, 2, 4});
    float data2[] = {
        15, 16, 17, 18, 19,
        10, 11, 12, 13, 14,
         5,  6,  7,  8,  9,
         0,  1,  2,  3,  4
    };
    nd_vector_impl<3, float, false> ndv2(data2, {2, 2, 5});

    float data_expected[] = {
        15, 17, 19, 21,
        15, 17, 19, 21,
        15, 17, 19, 21,
        15, 17, 19, 21
    };
    nd_vector_impl<3, float, false> ndv_expected(data_expected, {2, 2, 4});

    ndv1 += ndv2;

    EXPECT_EQ(ndv_expected, ndv1);
    EXPECT_NE(ndv_expected, ndv2);
}
TEST(nd_vector, operator_plus) {
    float data1[] = {
         0,  1,  2,  3,
         5,  6,  7,  8,
        10, 11, 12, 13,
        15, 16, 17, 18
    };
    nd_vector_impl<3, float, false> ndv1(data1, {2, 2, 4});
    float data2[] = {
        15, 16, 17, 18, 19,
        10, 11, 12, 13, 14,
         5,  6,  7,  8,  9,
         0,  1,  2,  3,  4
    };
    nd_vector_impl<3, float, false> ndv2(data2, {2, 2, 5});

    float data_expected[] = {
        15, 17, 19, 21,
        15, 17, 19, 21,
        15, 17, 19, 21,
        15, 17, 19, 21
    };
    nd_vector_impl<3, float, false> ndv_expected(data_expected, {2, 2, 4});

    auto ndv_result = ndv1 + ndv2;

    EXPECT_EQ(ndv_expected, ndv_result);
    EXPECT_NE(ndv_expected, ndv1);
    EXPECT_NE(ndv_expected, ndv2);
}

TEST(nd_vector, operator_minus_equals) {
    float data1[] = {
         0,  1,  2,  3,  4,
         5,  6,  7,  8,  9,
        10, 11, 12, 13, 14,
        15, 16, 17, 18, 19
    };
    nd_vector_impl<3, float, false> ndv1(data1, {2, 2, 5});
    float data2[] = {
        15, 16, 17, 18, 19,
        10, 11, 12, 13, 14,
         5,  6,  7,  8,  9,
         0,  1,  2,  3,  4
    };
    nd_vector_impl<3, float, false> ndv2(data2, {2, 2, 5});

    float data_expected[] = {
        -15, -15, -15, -15, -15,
        - 5, - 5, - 5, - 5, - 5,
          5,   5,   5,   5,   5,
         15,  15,  15,  15,  15
    };
    nd_vector_impl<3, float, false> ndv_expected(data_expected, {2, 2, 5});

    ndv1 -= ndv2;

    EXPECT_EQ(ndv_expected, ndv1);
    EXPECT_NE(ndv_expected, ndv2);
}
TEST(nd_vector, operator_minus) {
    float data1[] = {
         0,  1,  2,  3,  4,
         5,  6,  7,  8,  9,
        10, 11, 12, 13, 14,
        15, 16, 17, 18, 19
    };
    nd_vector_impl<3, float, false> ndv1(data1, {2, 2, 5});
    float data2[] = {
        15, 16, 17, 18, 19,
        10, 11, 12, 13, 14,
         5,  6,  7,  8,  9,
         0,  1,  2,  3,  4
    };
    nd_vector_impl<3, float, false> ndv2(data2, {2, 2, 5});

    float data_expected[] = {
        -15, -15, -15, -15, -15,
        - 5, - 5, - 5, - 5, - 5,
          5,   5,   5,   5,   5,
         15,  15,  15,  15,  15
    };
    nd_vector_impl<3, float, false> ndv_expected(data_expected, {2, 2, 5});

    auto ndv_result = ndv1 - ndv2;

    EXPECT_EQ(ndv_expected, ndv_result);
    EXPECT_NE(ndv_expected, ndv1);
    EXPECT_NE(ndv_expected, ndv2);
}

TEST(nd_vector, operator_multiply_equals) {
    float data1[] = {
         0,  1,  2,  3,  4,
         5,  6,  7,  8,  9,
        10, 11, 12, 13, 14,
        15, 16, 17, 18, 19
    };
    nd_vector_impl<3, float, false> ndv1(data1, {2, 2, 5});
    float data2[] = {
        15, 16, 17, 18, 19,
        10, 11, 12, 13, 14,
         5,  6,  7,  8,  9,
         0,  1,  2,  3,  4
    };
    nd_vector_impl<3, float, false> ndv2(data2, {2, 2, 5});

    float data_expected[] = {
         0, 16, 34,  54,  76,
        50, 66, 84, 104, 126,
        50, 66, 84, 104, 126,
         0, 16, 34,  54,  76
    };
    nd_vector_impl<3, float, false> ndv_expected(data_expected, {2, 2, 5});

    ndv1 *= ndv2;

    EXPECT_EQ(ndv_expected, ndv1);
    EXPECT_NE(ndv_expected, ndv2);
}
TEST(nd_vector, operator_multiply) {
    float data1[] = {
         0,  1,  2,  3,  4,
         5,  6,  7,  8,  9,
        10, 11, 12, 13, 14,
        15, 16, 17, 18, 19
    };
    nd_vector_impl<3, float, false> ndv1(data1, {2, 2, 5});
    float data2[] = {
        15, 16, 17, 18, 19,
        10, 11, 12, 13, 14,
         5,  6,  7,  8,  9,
         0,  1,  2,  3,  4
    };
    nd_vector_impl<3, float, false> ndv2(data2, {2, 2, 5});

    float data_expected[] = {
         0, 16, 34,  54,  76,
        50, 66, 84, 104, 126,
        50, 66, 84, 104, 126,
         0, 16, 34,  54,  76
    };
    nd_vector_impl<3, float, false> ndv_expected(data_expected, {2, 2, 5});

    auto ndv_result = ndv1 * ndv2;

    EXPECT_EQ(ndv_expected, ndv_result);
    EXPECT_NE(ndv_expected, ndv1);
    EXPECT_NE(ndv_expected, ndv2);
}

TEST(nd_vector, operator_divide_equals) {
    float data1[] = {
         0,  1,  2,  3,  4,
         5,  6,  7,  8,  9,
        10, 11, 12, 13, 14,
        15, 16, 17, 18, 19
    };
    nd_vector_impl<3, float, false> ndv1(data1, {2, 2, 5});
    float data2[] = {
        42,  1,  2,  3,
         5,  6,  7,  8,
        10, 11, 12, 13,
        15, 16, 17, 18
    };
    nd_vector_impl<3, float, false> ndv2(data2, {2, 2, 4});

    float data_expected[] = {
        0, 1, 1, 1,  4,
        1, 1, 1, 1,  9,
        1, 1, 1, 1, 14,
        1, 1, 1, 1, 19
    };
    nd_vector_impl<3, float, false> ndv_expected(data_expected, {2, 2, 5});

    ndv1 /= ndv2;

    EXPECT_EQ(ndv_expected, ndv1);
    EXPECT_NE(ndv_expected, ndv2);
}
TEST(nd_vector, operator_divide) {
    float data1[] = {
         0,  1,  2,  3,  4,
         5,  6,  7,  8,  9,
        10, 11, 12, 13, 14,
        15, 16, 17, 18, 19
    };
    nd_vector_impl<3, float, false> ndv1(data1, {2, 2, 5});
    float data2[] = {
        42,  1,  2,  3,
         5,  6,  7,  8,
        10, 11, 12, 13,
        15, 16, 17, 18
    };
    nd_vector_impl<3, float, false> ndv2(data2, {2, 2, 4});

    float data_expected[] = {
        0, 1, 1, 1,  4,
        1, 1, 1, 1,  9,
        1, 1, 1, 1, 14,
        1, 1, 1, 1, 19
    };
    nd_vector_impl<3, float, false> ndv_expected(data_expected, {2, 2, 5});

    auto ndv_result = ndv1 / ndv2;

    EXPECT_EQ(ndv_expected, ndv_result);
    EXPECT_NE(ndv_expected, ndv1);
    EXPECT_NE(ndv_expected, ndv2);
}

TEST(nd_vector, operator_plus_equals_scalar) { GTEST_SKIP(); }
TEST(nd_vector, operator_plus_scalar) { GTEST_SKIP(); }

TEST(nd_vector, operator_minus_equals_scalar) { GTEST_SKIP(); }
TEST(nd_vector, operator_minus_scalar) { GTEST_SKIP(); }

TEST(nd_vector, operator_multiply_equals_scalar) { GTEST_SKIP(); }
TEST(nd_vector, operator_multiply_scalar) { GTEST_SKIP(); }

TEST(nd_vector, operator_divide_equals_scalar) { GTEST_SKIP(); }
TEST(nd_vector, operator_divide_scalar) { GTEST_SKIP(); }
