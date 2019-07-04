#include <gtest/gtest.h>

#include "../src/statistics.hpp"

using namespace spice;

TEST(histogram, single_channel_impulse) {
    image<float> black(10, 10, {channel_names::ALPHA});
    auto hist_black = statistics::histogram(black, 42);

    EXPECT_EQ(1, hist_black.size());
    EXPECT_EQ(42, hist_black[0].size());
    EXPECT_EQ(100, hist_black[0][0]);
    for (size_t sample = 1; sample < hist_black[0].size(); ++sample)
        EXPECT_EQ(hist_black[0][sample], 0);
}
