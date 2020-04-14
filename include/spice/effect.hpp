/**
 * \file
 * This header defines a collection of blurring and averaging functions.
 */
#ifndef SPICE_EFFECT
#define SPICE_EFFECT

#define _USE_MATH_DEFINES
#include <cmath>

#include "image.hpp"
#include "print.hpp"

namespace spice {
/**
 * Special use effects like simulations of physical filters or lens aberrations
 * are grouped together in this namespace.
 */
namespace effect {
namespace
{
    const auto sqrt_2_pi = std::sqrt(2 * M_PI);
    [[nodiscard]] float gaussian_fn(float std_deviation, long x) {
        return (1 / (sqrt_2_pi * std_deviation)) * std::pow(
            M_E,
            -((x * x) / (2 * std_deviation * std_deviation)));
    }
}

/**
 * \brief Imitates filters like the Tiffen Black Pro Mist or Schneider Hollywood
 * Black Magic
 * 
 * In a nutshell, this filter applies a Gaussian blur, but instead of averaging
 * with only the weights indicated by the Gaussian kernel, those weights are
 * multiplied by a power of the initial pixel value (relative to the maximum
 * value).
 * 
 * \todo generalise this to non-floating point types
 * 
 * \param input The image to treat
 * \param std_deviation_x The standard deviation of the gaussian filter in the
 * horizontal direction
 * \param std_deviation_y The standard deviation of the gaussian filter in the
 * vertical direction
 * \param intensity A scale factor to apply to the effect to render it more
 * subtle or more prevalent
 * \param exponent Regulates the shape of the falloff by multiplying the
 * Gaussian weigths `exponent` times with the relative pixel value
 */
template<typename T,
    std::enable_if_t<std::is_floating_point<T>::value, int> = 0>
[[nodiscard]] image<T> magic_mist(
    image<T> const & input,
    float std_deviation_x,
    float std_deviation_y,
    float intensity,
    size_t exponent = 3)
{
    // create the output image
    image<T> out(input.width(), input.height(), input.channel_semantics());

    long radius_x = std::ceil(std_deviation_x * 3);
    long radius_y = std::ceil(std_deviation_y * 3);

    // bloom vertically
    for (long x = 0; x < out.width(); ++x) {
        for (long y = 0; y < out.height(); ++y) {
            for (long bloom_y = std::max(0L, y - radius_y);
                bloom_y < std::min(static_cast<long>(input.height()), y + radius_y);
                ++bloom_y)
            {
                auto in = input(x, bloom_y);
                auto gauss = gaussian_fn(std_deviation_y, bloom_y - y);
                out(x, y) += gauss * in * in * in;
            }
        }
    }

    image<T> out2(input.width(), input.height(), input.channel_semantics());

    // bloom horizontally
    for (long x = 0; x < out.width(); ++x) {
        for (long y = 0; y < out.height(); ++y) {
            for (long bloom_x = std::max(0L, x - radius_x);
                bloom_x < std::min(static_cast<long>(input.width()), x + radius_x);
                ++bloom_x)
            {
                auto in = out(bloom_x, y);
                auto gauss = gaussian_fn(std_deviation_x, bloom_x - x);
                out2(x, y) += gauss * in * intensity;
            }
        }
    }

    for (size_t i = 0; i < out2.data().size(); ++i) {
        out2.data()[i] += input.data()[i];
    }

    return out2;
}
}
}

#endif // SPICE_EFFECT
