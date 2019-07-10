#include <gtest/gtest.h>
#include <type_traits>
#import <limits>
#import <numeric>
#import <cstdint>
#include "../src/image.hpp"
#include "../src/statistics.hpp"
#include "../src/print.hpp"

using namespace spice;

template<typename T>
image<T> make_checkerboard(size_t width = 1,
    size_t height = 1,
    channel_list channel_semantics = { "R", "G", "B" })
{
    image<T> im(width, height, channel_semantics);
    for (size_t pxl = 0; pxl < im.data().size(); pxl += im.channels())
        for (size_t chan = 0; chan < im.channels(); ++chan)
            im.data()[pxl + chan] = (pxl / 3) % 2 == 0 ?
                image<T>::intensity_range.max :
                image<T>::intensity_range.min;

    return im;
}

TEST(vector_like_operators, operator_equals_color) {
    color<float> c1({0.5, 0.42, 0.47});
    color<float> c2({0.5, 0.42, 0.47});
    color<float> c3({0.5, 0.42, 0.47, 0.1});
    color<float> c4({0.47, 0.42, 0.47});

    EXPECT_TRUE(c1 == c2);
    EXPECT_TRUE(c2 == c1);

    EXPECT_FALSE(c1 == c3);
    EXPECT_FALSE(c3 == c1);
    EXPECT_FALSE(c1 == c4);
    EXPECT_FALSE(c4 == c1);
}

TEST(vector_like_operators, operator_not_equals_color) {
    color<float> c1({0.5, 0.42, 0.47});
    color<float> c2({0.5, 0.42, 0.47});
    color<float> c3({0.5, 0.42, 0.47, 0.1});
    color<float> c4({0.47, 0.42, 0.47});

    EXPECT_FALSE(c1 != c2);
    EXPECT_FALSE(c2 != c1);

    EXPECT_TRUE(c1 != c3);
    EXPECT_TRUE(c3 != c1);
    EXPECT_TRUE(c1 != c4);
    EXPECT_TRUE(c4 != c1);
}

TEST(vector_like_operators, operator_plus_equals_color) {
    color<float> c1({0.5, 0.42, 0.47});
    color<float> c2({0.5, 0.05, 0.1});

    c1 += c2;

    EXPECT_EQ(c1[0], 1.f);
    EXPECT_EQ(c1[1], 0.47f);
    EXPECT_EQ(c1[2], 0.57f);
}

TEST(vector_like_operators, operator_plus_color_pixelview) {
    color<float> c1({0.5, 0.42, 0.47});
    std::vector<float> d2{0.5, 0.05, 0.1};
    pixel_view<float> pv2(&d2[0], 3);

    auto c3 = c1 + pv2;
    auto pv3 = pv2 + c1;

    EXPECT_EQ(c1, color<float>({0.5, 0.42, 0.47}));
    EXPECT_NE(c1, c3);
    // pv3 should be a copy of pv2 but their underlying data is the same, so
    // they still equal one another after the addition.
    EXPECT_EQ(pv2, pv3);
    EXPECT_NE(&pv2, &pv3);
    EXPECT_EQ(pv2, pixel_view<float>(&d2[0], 3));

    EXPECT_EQ(pv3, c3);

    EXPECT_EQ(c3[0], 1.f);
    EXPECT_EQ(c3[1], 0.47f);
    EXPECT_EQ(c3[2], 0.57f);

    EXPECT_EQ(pv3[0], 1.f);
    EXPECT_EQ(pv3[1], 0.47f);
    EXPECT_EQ(pv3[2], 0.57f);
}

TEST(vector_like_operators, operator_equals_color_pixel_view) {
    color<float> c1({0.5, 0.42, 0.47});
    color<float> c2({0.5, 0.42, 0.47});
    color<float> c3({0.5, 0.42, 0.47, 0.1});
    color<float> c4({0.47, 0.42, 0.47});

    std::vector<float> d1{0.5, 0.42, 0.47};
    std::vector<float> d2{0.5, 0.42, 0.47};
    std::vector<float> d3{0.5, 0.42, 0.47, 0.1};
    std::vector<float> d4{0.47, 0.42, 0.47};

    pixel_view<float> pv1(&d1[0], 3);
    pixel_view<float> pv2(&d2[0], 3);
    pixel_view<float> pv3(&d3[0], 4);
    pixel_view<float> pv4(&d4[0], 3);

    EXPECT_TRUE(pv1 == c2);
    EXPECT_TRUE(pv2 == c1);

    EXPECT_FALSE(pv1 == c3);
    EXPECT_FALSE(pv3 == c1);
    EXPECT_FALSE(pv1 == c4);
    EXPECT_FALSE(pv4 == c1);

    EXPECT_TRUE(c1 == pv2);
    EXPECT_TRUE(c2 == pv1);

    EXPECT_FALSE(c1 == pv3);
    EXPECT_FALSE(c3 == pv1);
    EXPECT_FALSE(c1 == pv4);
    EXPECT_FALSE(c4 == pv1);
}

TEST(vector_like_operators, operator_equals_pixel_view) {
    std::vector<float> d1{0.5, 0.42, 0.47};
    std::vector<float> d2{0.5, 0.42, 0.47};
    std::vector<float> d3{0.5, 0.42, 0.47, 0.1};
    std::vector<float> d4{0.47, 0.42, 0.47};

    pixel_view<float> pv1(&d1[0], 3);
    pixel_view<float> pv2(&d2[0], 3);
    pixel_view<float> pv3(&d3[0], 4);
    pixel_view<float> pv4(&d4[0], 3);

    EXPECT_TRUE(pv1 == pv2);
    EXPECT_TRUE(pv2 == pv1);

    EXPECT_FALSE(pv1 == pv3);
    EXPECT_FALSE(pv3 == pv1);
    EXPECT_FALSE(pv1 == pv4);
    EXPECT_FALSE(pv4 == pv1);
}

TEST(color, operator_subscript) {
    color<float> c({0.5, 0.42, 0.47, 0.1});

    EXPECT_EQ(c[0], 0.5f);
    EXPECT_EQ(c[1], 0.42f);
    EXPECT_EQ(c[2], 0.47f);
    EXPECT_EQ(c[3], 0.1f);

    c[0] = 3.1415;

    EXPECT_EQ(c[0], 3.1415f);
}

TEST(color, operator_subscript_const) {
    const color<float> c({0.5, 0.42, 0.47, 0.1});

    EXPECT_EQ(c[0], 0.5f);
    EXPECT_EQ(c[1], 0.42f);
    EXPECT_EQ(c[2], 0.47f);
    EXPECT_EQ(c[3], 0.1f);

    auto & first = c[0];
    static_assert(
        std::is_const<std::remove_reference<decltype(first)>::type>::value,
        "color::operator[] const does not return const value");
}

TEST(image, default_constructor) {
    image<float> im;

    EXPECT_EQ(0, im.data().size());
    EXPECT_EQ(0, im.width());
    EXPECT_EQ(0, im.height());
    EXPECT_EQ(0, im.channels());
    EXPECT_EQ(0, im.channel_semantics().size());
}

TEST(image, size_constructor) {
    image<float> im1(2, 3, { "R", "G", "B", "A" });

    EXPECT_EQ(24, im1.data().size());
    for (auto d : im1.data()) {
        EXPECT_EQ(0.f, d);
    }
    EXPECT_EQ(2, im1.width());
    EXPECT_EQ(3, im1.height());
    EXPECT_EQ(4, im1.channels());
    EXPECT_EQ(4, im1.channel_semantics().size());
    EXPECT_EQ("R", im1.channel_semantics()[0]);
    EXPECT_EQ("G", im1.channel_semantics()[1]);
    EXPECT_EQ("B", im1.channel_semantics()[2]);
    EXPECT_EQ("A", im1.channel_semantics()[3]);
}

TEST(image, copy_constructor) {
    image<float> im1(2, 3, {
        "R",
        "G",
        "B",
        "A"
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
    EXPECT_EQ("R", im2.channel_semantics()[0]);
    EXPECT_EQ("G", im2.channel_semantics()[1]);
    EXPECT_EQ("B", im2.channel_semantics()[2]);
    EXPECT_EQ("A", im2.channel_semantics()[3]);
}

TEST(image, operator_equals) {
    image<float> im1(2, 3, {
        "R",
        "G",
        "B",
        "A"
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
    EXPECT_EQ("R", im2.channel_semantics()[0]);
    EXPECT_EQ("G", im2.channel_semantics()[1]);
    EXPECT_EQ("B", im2.channel_semantics()[2]);
    EXPECT_EQ("A", im2.channel_semantics()[3]);

    im2(0, 0, 0) = 0.42;

    EXPECT_FALSE(im1 == im2);
    EXPECT_TRUE(im1 != im2);

    image<float> im3(4, 1);
    image<float> im4(1, 4);

    EXPECT_FALSE(im3 == im4);
    EXPECT_TRUE(im3 != im4);

    image<float> im5(2, 2, { "Y", "U", "V" });
    image<float> im6(2, 2);

    EXPECT_FALSE(im5 == im6);
    EXPECT_TRUE(im3 != im6);
}

TEST(image, operator_call_two_arg) {
    auto im = make_checkerboard<float>(2, 2, { "Y", "U", "V" });

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
    auto im = make_checkerboard<float>(2, 2, { "Y", "U", "V" });

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
    auto im = make_checkerboard<float>(2, 2, { "Y", "U", "V" });

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
    auto im = make_checkerboard<float>(2, 2, { "Y", "U", "V" });

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

TEST(image_helpers, type_to_typedesc) {
    static_assert(helpers::type_to_typedesc<double>() ==
        OIIO::TypeDesc::DOUBLE,
        "type_to_typedesc does not return DOUBLE for input of double");

    static_assert(helpers::type_to_typedesc<float>() ==
        OIIO::TypeDesc::FLOAT,
        "type_to_typedesc does not return FLOAT for input of float");

    static_assert(helpers::type_to_typedesc<uint32_t>() ==
        OIIO::TypeDesc::UINT,
        "type_to_typedesc does not return UINT for input of uint32_t");

    static_assert(helpers::type_to_typedesc<uint16_t>() ==
        OIIO::TypeDesc::UINT16,
        "type_to_typedesc does not return UINT16 for input of uint16_t");

    static_assert(helpers::type_to_typedesc<uint8_t>() ==
        OIIO::TypeDesc::UINT8,
        "type_to_typedesc does not return UINT8 for input of uint8_t");
}

TEST(image_support, load_image) {
    auto boat = load_image<float>("../data/testing/boat.jpg");

    auto hist = statistics::histogram(boat, 50);

    // print histogram for debugging
    // spice::print::histogram(hist, 100, {
    //     color<float>({1, 0, 0}),
    //     color<float>({0, 1, 0}),
    //     color<float>({0, 0, 1})
    // });

    EXPECT_EQ(boat.channel_semantics(),
        std::vector<std::string>({ "R", "G", "B" }));
    EXPECT_EQ(boat.width(), 512);
    EXPECT_EQ(boat.height(), 410);

    size_t red_average = std::accumulate(
        hist[0].begin(),
        hist[0].end(),
        0)/hist[0].size();
    EXPECT_GT(red_average, 0);
    EXPECT_LT(red_average, 1500);
    size_t green_average = std::accumulate(
        hist[1].begin(),
        hist[1].end(),
        0)/hist[1].size();
    EXPECT_GT(green_average, 0);
    EXPECT_LT(green_average, 1500);
    size_t blue_average = std::accumulate(
        hist[2].begin(),
        hist[2].end(),
        0)/hist[2].size();
    EXPECT_GT(blue_average, 0);
    EXPECT_LT(blue_average, 1500);
}

TEST(image_support, write_image) {
    auto boat = load_image<float>("../data/testing/boat.jpg");

    bool written = write_image("../data/testing/boat_2.jpg",
        boat, OIIO::TypeDesc::UINT8);

    EXPECT_TRUE(written);
}
