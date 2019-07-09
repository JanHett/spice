#include <gtest/gtest.h>

#include "../src/statistics.hpp"

using namespace spice;

TEST(histogram, single_channel_impulse_black) {
    // Black image
    image<float> black(10, 10, {"ALPHA"});
    auto hist_black = statistics::histogram(black, 42);

    EXPECT_EQ(1, hist_black.size());
    EXPECT_EQ(42, hist_black[0].size());
    EXPECT_EQ(100, hist_black[0][0]);
    for (size_t sample = 1; sample < hist_black[0].size(); ++sample)
        EXPECT_EQ(hist_black[0][sample], 0);
}

TEST(histogram, single_channel_impulse_white) {
    image<uint16_t> white(10, 42, {"ALPHA"});
    for (auto & smpl: white.data())
        smpl = image<uint16_t>::intensity_range.max;

    auto hist_white = statistics::histogram(white, 47);

    EXPECT_EQ(1, hist_white.size());
    EXPECT_EQ(47, hist_white[0].size());
    EXPECT_EQ(420, hist_white[0][46]);
    for (size_t sample = 0; sample < hist_white[0].size() - 1; ++sample)
        EXPECT_EQ(hist_white[0][sample], 0);
}

TEST(histogram, single_channel_impulse_grey) {
    image<uint8_t> grey(10, 42, {"ALPHA"});
    uint8_t grey_val = image<uint8_t>::intensity_range.max / 2;
    for (auto & smpl: grey.data())
        smpl = grey_val;

    auto hist_grey = statistics::histogram(grey, 100);

    EXPECT_EQ(1, hist_grey.size());
    EXPECT_EQ(100, hist_grey[0].size());
    EXPECT_EQ(420, hist_grey[0][49]);
    for (size_t sample = 0; sample < hist_grey[0].size(); ++sample)
    {
        if (sample == 49) continue;
        EXPECT_EQ(hist_grey[0][sample], 0);
    }
}

TEST(histogram, single_channel_overflow_underflow) {
    image<double> grey(10, 42, {"ALPHA"});
    double grey_val = image<double>::intensity_range.max / 2;
    for (auto & smpl: grey.data())
        smpl = grey_val;

    // introduce values that are outside the intensity range of the image
    grey(0, 0, 0) = -1;
    grey(0, 1, 0) = 1.47;

    auto hist_grey = statistics::histogram(grey, 100);

    EXPECT_EQ(1, hist_grey.size());
    EXPECT_EQ(100, hist_grey[0].size());
    // should be all grey except for one sample clipped to black and another
    // clipped to white
    EXPECT_EQ(418, hist_grey[0][50]);
    EXPECT_EQ(1, hist_grey[0][0]);
    EXPECT_EQ(1, hist_grey[0][99]);
    // check that the rest of the samples are actually all 0
    for (size_t sample = 1; sample < hist_grey[0].size() - 1; ++sample)
    {
        if (sample == 50) continue; // skip the grey part
        EXPECT_EQ(hist_grey[0][sample], 0);
    }
}
