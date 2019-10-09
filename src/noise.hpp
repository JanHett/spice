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
 */
namespace noise {
    namespace {
        std::random_device rand_dev;
    }
    /**
     * Adds salt and pepper noise to the input image.
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
     * Adds salt and pepper noise to a copy of the input image.
     *
     * \param source The image to modify
     * \param density The probability of a pixel being either white or black
     * \returns The modified image
     */
    // template<typename T>
    // [[nodiscard]] image<T> salt_and_pepper(
    //     image<T> source,
    //     float density)
    // {
    //     salt_and_pepper(source, density);
    //     return source;
    // }

    /**
     * Adds uniform noise to the input image.
     *
     * \param source The image to modify
     * \param sigma The standard deviation
     * \param mean The mean of the noise distribution
     */
    template<typename T>
    void uniform(
        image<T> & source,
        float sigma,
        T const & mean);

    /**
     * Adds uniform noise to a copy of the input image.
     *
     * \param source The image to modify
     * \param sigma The standard deviation
     * \param mean The mean of the noise distribution
     * \returns The modified image
     */
    template<typename T>
    image<T> uniform(
        image<T> source,
        float sigma,
        T const & mean)
    {
        uniform(source, sigma, mean);
        return source;
    }

    /**
     * Adds gaussian noise to the input image.
     *
     * \param source The image to modify
     * \param sigma The standard deviation
     * \param mean The mean of the noise distribution
     */
    template<typename T>
    void gaussian(
        image<T> & source,
        float sigma,
        T const & mean);

    /**
     * Adds gaussian noise to a copy of the input image.
     *
     * \param source The image to modify
     * \param sigma The standard deviation
     * \param mean The mean of the noise distribution
     * \returns The modified image
     */
    template<typename T>
    image<T> gaussian(
        image<T> source,
        float sigma,
        T const & mean)
    {
        gaussian(source, sigma, mean);
        return source;
    }
}
}

#endif // SPICE_NOISE
