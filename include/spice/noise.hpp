/**
 * \file
 * This header defines a number of algorithms for adding noise.
 * 
 */
#ifndef SPICE_NOISE
#define SPICE_NOISE

#include <random>

#include "image.hpp"

namespace spice {
/**
 * This namespace exports various noise generation algorithms.
 *
 * \note By default, none of these algorithms checks for overflow. This is a
 * problem especially with integer types, but much less so with floating point
 * types since the latter do not clip beyond their interpreted range.
 */
namespace noise {
    namespace {
        std::random_device rand_dev;
    }
    /**
     * Adds salt and pepper noise to the input image.
     * 
     * \image html ./showcase/noise_salt_and_pepper.jpg
     *
     * \param source The image to modify
     * \param density The probability of a pixel being either white or black
     */
    template<typename T>
    void salt_and_pepper(
        image<T> & source,
        float density)
    {
        std::mt19937 mersenne(rand_dev());
        std::uniform_real_distribution<float> fl_dist(0.f,
            std::nextafterf(1.f, 1.1));
        std::uniform_int_distribution<int> int_dist(0, 1);
        for(size_t x = 0; x < source.width(); ++x) {
            for(size_t y = 0; y < source.height(); ++y) {
                if (fl_dist(mersenne) < density) {
                    if (int_dist(mersenne))
                        source(x, y) = image<T>::intensity_range.max;
                    else
                        source(x, y) = image<T>::intensity_range.min;
                }
            }
        }
    }

    /**
     * Adds uniform noise to the input image.
     * 
     * \image html ./showcase/noise_uniform.jpg
     *
     * \param source The image to modify
     * \param min The minimum noise value
     * \param max The maximum noise value
     * \param operation The with which to combine the noise with the
     * image. Please note that the image element will be the first operand and
     * the noise sample the second (i.e. `element = operation(element, noise`).
     */
    template<typename T, typename Op = std::plus<T>>
    void uniform(
        image<T> & source,
        T const & min,
        T const & max,
        Op operation = Op())
    {
        std::mt19937 mersenne(rand_dev());
        if constexpr (std::is_floating_point<T>::value) {
            std::uniform_real_distribution<T> dist(min, max);
            for(auto & elem : source.data())
                elem = operation(elem, dist(mersenne));
        } else {
            std::uniform_int_distribution<T> dist(min, max);
            for(auto & elem : source.data())
                elem = operation(elem, dist(mersenne));
        }
    }

    /**
     * Adds gaussian noise to the input image.
     * 
     * \image html ./showcase/noise_gaussian.jpg
     *
     * \param source The image to modify
     * \param mean The mean of the noise distribution
     * \param sigma The standard deviation
     * \param operation The operation with which to combine the noise with the
     * image. Please note that the image element will be the first operand and
     * the noise sample the second (i.e. `element = operation(element, noise`).
     */
    template<typename T, typename Op = std::plus<T>>
    void gaussian(
        image<T> & source,
        T const & mean,
        T const & sigma,
        Op operation = Op())
    {
        std::mt19937 mersenne(rand_dev());
        std::normal_distribution<float> normal_dist(mean, sigma);
        for(auto & elem : source.data())
            elem = operation(elem, normal_dist(mersenne));
    }
}
}

#endif // SPICE_NOISE
