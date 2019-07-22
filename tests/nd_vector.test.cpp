#include <gtest/gtest.h>

#include "../src/nd_vector.hpp"

TEST(nd_vector, default_constructor) {}
TEST(nd_vector, copy_constructor) {}
TEST(nd_vector, data_constructor) {}

TEST(nd_vector, destructor_owner) {}
TEST(nd_vector, destructor_non_owner) {}

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
