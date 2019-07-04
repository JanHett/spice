#include <gtest/gtest.h>

#include "../src/statistics.hpp"

using namespace spice;

TEST(histogram, single_channel_impulse) {
    // Black image
    image<float> black(10, 10, {channel_names::ALPHA});
    auto hist_black = statistics::histogram(black, 42);

    EXPECT_EQ(1, hist_black.size());
    EXPECT_EQ(42, hist_black[0].size());
    EXPECT_EQ(100, hist_black[0][0]);
    for (size_t sample = 1; sample < hist_black[0].size(); ++sample)
        EXPECT_EQ(hist_black[0][sample], 0);

    // White image
    image<uint16_t> white(10, 42, {channel_names::ALPHA});
    for (auto & smpl: white.data())
        smpl = image<uint16_t>::intensity_range.max;

    auto hist_white = statistics::histogram(white, 47);

    EXPECT_EQ(1, hist_white.size());
    EXPECT_EQ(47, hist_white[0].size());
    EXPECT_EQ(420, hist_white[0][46]);
    for (size_t sample = 0; sample < hist_white[0].size() - 1; ++sample)
        EXPECT_EQ(hist_white[0][sample], 0);
}
