#include <gtest/gtest.h>

#include <type_traits>
#include <limits>
#include <numeric>
#include <cstdint>
#include <fstream>

#include <gsl/span>

#include <spice/image.hpp>
#include <spice/statistics.hpp>
#include <spice/print.hpp>

using namespace spice;

template<typename T>
image<T> make_checkerboard(size_t width = 1,
    size_t height = 1,
    channel_list channel_semantics = { "R", "G", "B" })
{
    image<T> im(width, height, channel_semantics);
    for (typename gsl::span<T>::index_type pxl = 0; pxl < im.data().size();\
        pxl += im.channels())
        for (size_t chan = 0; chan < im.channels(); ++chan)
            im.data()[pxl + chan] = (pxl / 3) % 2 == 0 ?
                image<T>::intensity_range.max :
                image<T>::intensity_range.min;

    return im;
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

TEST(image_support, transpose) {
    int * data_horizontal = new int[18] {
         0,  1,  2,
         3,  4,  5,
         6,  7,  8,
         9, 10, 11,
        12, 13, 14,
        15, 16, 17,
    };
    int * data_vertical = new int[18] {
         0,  1,  2,
         3,  4,  5,
         6,  7,  8,
         9, 10, 11,
        12, 13, 14,
        15, 16, 17,
    };
    image<int> i_horizontal(data_horizontal, 3,2, {"R","G","B"});
    image<int> i_vertical(data_vertical, 2,3, {"R","G","B"});

    auto tp_horizontal = transpose(i_horizontal);
    auto tp_vertical = transpose(i_vertical);

    EXPECT_EQ(2, tp_horizontal.width());
    EXPECT_EQ(3, tp_horizontal.height());

    EXPECT_EQ(3, tp_vertical.width());
    EXPECT_EQ(2, tp_vertical.height());
    
    int * tp_data_horizontal = new int[18] {
         0,  1,  2,
         6,  7,  8,
        12, 13, 14,
         3,  4,  5,
         9, 10, 11,
        15, 16, 17
    };
    int * tp_data_vertical = new int[18] {
         0,  1,  2,
         9, 10, 11,
         3,  4,  5,
        12, 13, 14,
         6,  7,  8,
        15, 16, 17
    };
    image<int> i_control_horizontal(tp_data_horizontal, 2,3, {"R","G","B"});
    image<int> i_control_vertical(tp_data_vertical, 3,2, {"R","G","B"});

    EXPECT_EQ(i_control_horizontal, tp_horizontal);
    EXPECT_EQ(i_control_vertical, tp_vertical);
}

TEST(image_support, load_image) {
    auto boat = load_image<float>("../data/testing/boat.jpg");

    auto hist = statistics::histogram(boat, 50);

    // print histogram for debugging
    // print::histogram(hist, 100, {
    //     color<float>({1, 0, 0}),
    //     color<float>({0, 1, 0}),
    //     color<float>({0, 0, 1})
    // });

    // print image for debugging
    // print::image(boat, 10);

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

    std::string out_path("../data/testing/boat_2.jpg");
    bool written = write_image(out_path.c_str(),
        boat, OIIO::TypeDesc::UINT8);

    EXPECT_TRUE(written);
    std::ifstream f(out_path.c_str());
    EXPECT_TRUE(f.good());

    std::remove(out_path.c_str());
}
