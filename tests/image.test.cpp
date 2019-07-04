#include <gtest/gtest.h>
#include <type_traits>
#import <limits>
#import <cstdint>
#include "../src/image.hpp"

using namespace spice;

template<typename T>
image<T> make_checkerboard(size_t width = 1,
    size_t height = 1,
    channel_list channel_semantics = {
        channel_names::RED,
        channel_names::GREEN,
        channel_names::BLUE
    })
{
    image<T> im(width, height, channel_semantics);
    for (size_t pxl = 0; pxl < im.data().size(); pxl += im.channels())
        for (size_t chan = 0; chan < im.channels(); ++chan)
            im.data()[pxl + chan] = (pxl / 3) % 2 == 0 ?
                image<T>::intensity_range.max :
                image<T>::intensity_range.min;

    return im;
}

TEST(image, default_constructor) {
    image<float> im;

    EXPECT_EQ(3, im.data().size());
    for (auto d : im.data()) {
        EXPECT_EQ(0.f, d);
    }
    EXPECT_EQ(1, im.width());
    EXPECT_EQ(1, im.height());
    EXPECT_EQ(3, im.channels());
    EXPECT_EQ(3, im.channel_semantics().size());
    EXPECT_EQ(channel_names::RED, im.channel_semantics()[0]);
    EXPECT_EQ(channel_names::GREEN, im.channel_semantics()[1]);
    EXPECT_EQ(channel_names::BLUE, im.channel_semantics()[2]);
}

TEST(image, size_constructor) {
    image<float> im1(2, 3, {
        channel_names::RED,
        channel_names::GREEN,
        channel_names::BLUE,
        channel_names::ALPHA
    });

    EXPECT_EQ(24, im1.data().size());
    for (auto d : im1.data()) {
        EXPECT_EQ(0.f, d);
    }
    EXPECT_EQ(2, im1.width());
    EXPECT_EQ(3, im1.height());
    EXPECT_EQ(4, im1.channels());
    EXPECT_EQ(4, im1.channel_semantics().size());
    EXPECT_EQ(channel_names::RED, im1.channel_semantics()[0]);
    EXPECT_EQ(channel_names::GREEN, im1.channel_semantics()[1]);
    EXPECT_EQ(channel_names::BLUE, im1.channel_semantics()[2]);
    EXPECT_EQ(channel_names::ALPHA, im1.channel_semantics()[3]);
}

TEST(image, copy_constructor) {
    image<float> im1(2, 3, {
        channel_names::RED,
        channel_names::GREEN,
        channel_names::BLUE,
        channel_names::ALPHA
    });
    image<float> im2(im1);

    EXPECT_TRUE(im1 == im2);

    EXPECT_EQ(24, im2.data().size());
    for (auto d : im2.data()) {
        EXPECT_EQ(0.f, d);
    }
    EXPECT_EQ(2, im2.width());
    EXPECT_EQ(3, im2.height());
    EXPECT_EQ(4, im2.channels());
    EXPECT_EQ(4, im2.channel_semantics().size());
    EXPECT_EQ(channel_names::RED, im2.channel_semantics()[0]);
    EXPECT_EQ(channel_names::GREEN, im2.channel_semantics()[1]);
    EXPECT_EQ(channel_names::BLUE, im2.channel_semantics()[2]);
    EXPECT_EQ(channel_names::ALPHA, im2.channel_semantics()[3]);
}

TEST(image, operator_equals) {
    image<float> im1(2, 3, {
        channel_names::RED,
        channel_names::GREEN,
        channel_names::BLUE,
        channel_names::ALPHA
    });
    image<float> im2(im1);

    EXPECT_TRUE(im1 == im2);
    EXPECT_FALSE(im1 != im2);

    EXPECT_EQ(24, im2.data().size());
    for (auto d : im2.data()) {
        EXPECT_EQ(0.f, d);
    }
    EXPECT_EQ(2, im2.width());
    EXPECT_EQ(3, im2.height());
    EXPECT_EQ(4, im2.channels());
    EXPECT_EQ(4, im2.channel_semantics().size());
    EXPECT_EQ(channel_names::RED, im2.channel_semantics()[0]);
    EXPECT_EQ(channel_names::GREEN, im2.channel_semantics()[1]);
    EXPECT_EQ(channel_names::BLUE, im2.channel_semantics()[2]);
    EXPECT_EQ(channel_names::ALPHA, im2.channel_semantics()[3]);

    im2(0, 0, 0) = 0.42;

    EXPECT_FALSE(im1 == im2);
    EXPECT_TRUE(im1 != im2);

    image<float> im3(4, 1);
    image<float> im4(1, 4);

    EXPECT_FALSE(im3 == im4);
    EXPECT_TRUE(im3 != im4);

    image<float> im5(2, 2, {
        channel_names::Y,
        channel_names::U,
        channel_names::V
    });
    image<float> im6(2, 2);

    EXPECT_FALSE(im5 == im6);
    EXPECT_TRUE(im3 != im6);
}

TEST(image, operator_call_two_arg) {
    auto im = make_checkerboard<float>(2, 2, {
        channel_names::Y,
        channel_names::U,
        channel_names::V
    });

    EXPECT_EQ(im(0, 0)[0], 1);
    EXPECT_EQ(im(0, 0)[1], 1);
    EXPECT_EQ(im(0, 0)[2], 1);
    EXPECT_EQ(im(1, 0)[0], 1);
    EXPECT_EQ(im(1, 0)[1], 1);
    EXPECT_EQ(im(1, 0)[2], 1);
    EXPECT_EQ(im(0, 1)[0], 0);
    EXPECT_EQ(im(0, 1)[1], 0);
    EXPECT_EQ(im(0, 1)[2], 0);
    EXPECT_EQ(im(1, 1)[0], 0);
    EXPECT_EQ(im(1, 1)[1], 0);
    EXPECT_EQ(im(1, 1)[2], 0);
}

TEST(image, operator_call_two_arg_const) {
    auto im = make_checkerboard<float>(2, 2, {
        channel_names::Y,
        channel_names::U,
        channel_names::V
    });

    auto const im_const(im);

    EXPECT_EQ(im_const(0, 0)[0], 1);
    EXPECT_EQ(im_const(0, 0)[1], 1);
    EXPECT_EQ(im_const(0, 0)[2], 1);
    EXPECT_EQ(im_const(1, 0)[0], 1);
    EXPECT_EQ(im_const(1, 0)[1], 1);
    EXPECT_EQ(im_const(1, 0)[2], 1);
    EXPECT_EQ(im_const(0, 1)[0], 0);
    EXPECT_EQ(im_const(0, 1)[1], 0);
    EXPECT_EQ(im_const(0, 1)[2], 0);
    EXPECT_EQ(im_const(1, 1)[0], 0);
    EXPECT_EQ(im_const(1, 1)[1], 0);
    EXPECT_EQ(im_const(1, 1)[2], 0);
}

TEST(image, operator_call_three_arg) {
    auto im = make_checkerboard<float>(2, 2, {
        channel_names::Y,
        channel_names::U,
        channel_names::V
    });

    EXPECT_EQ(im(0, 0, 0), 1);
    EXPECT_EQ(im(0, 0, 1), 1);
    EXPECT_EQ(im(0, 0, 2), 1);
    EXPECT_EQ(im(1, 0, 0), 1);
    EXPECT_EQ(im(1, 0, 1), 1);
    EXPECT_EQ(im(1, 0, 2), 1);
    EXPECT_EQ(im(0, 1, 0), 0);
    EXPECT_EQ(im(0, 1, 1), 0);
    EXPECT_EQ(im(0, 1, 2), 0);
    EXPECT_EQ(im(1, 1, 0), 0);
    EXPECT_EQ(im(1, 1, 1), 0);
    EXPECT_EQ(im(1, 1, 2), 0);
}

TEST(image, operator_call_three_arg_const) {
    auto im = make_checkerboard<float>(2, 2, {
        channel_names::Y,
        channel_names::U,
        channel_names::V
    });

    auto const im_const(im);

    EXPECT_EQ(im_const(0, 0, 0), 1);
    EXPECT_EQ(im_const(0, 0, 1), 1);
    EXPECT_EQ(im_const(0, 0, 2), 1);
    EXPECT_EQ(im_const(1, 0, 0), 1);
    EXPECT_EQ(im_const(1, 0, 1), 1);
    EXPECT_EQ(im_const(1, 0, 2), 1);
    EXPECT_EQ(im_const(0, 1, 0), 0);
    EXPECT_EQ(im_const(0, 1, 1), 0);
    EXPECT_EQ(im_const(0, 1, 2), 0);
    EXPECT_EQ(im_const(1, 1, 0), 0);
    EXPECT_EQ(im_const(1, 1, 1), 0);
    EXPECT_EQ(im_const(1, 1, 2), 0);
}

TEST(image, intensity_range) {
    // check that the intensity range is indeed constexpr
    static_assert(image<float>::intensity_range.min == 0,
        "image<float> intensity range does not bottom out at 0.");
    static_assert(image<float>::intensity_range.max == 1.f,
        "image<float> intensity range does not max out at 1.");

    // check various types' intensity ranges for correctness
    EXPECT_EQ(image<float>::intensity_range, (range<float>{0, 1.f}));

    EXPECT_EQ(image<double>::intensity_range, (range<double>{0, 1.0}));

    EXPECT_EQ(image<uint16_t>::intensity_range, (range<uint16_t>{
            std::numeric_limits<uint16_t>::min(),
            std::numeric_limits<uint16_t>::max()}));
}
