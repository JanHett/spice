/**
 * \file
 * This header defines a number of algorithms for adding noise.
 * 
 */
#ifndef SPICE_NOISE
#define SPICE_NOISE

#include "image.hpp"

namespace spice {
/**
 * This namespace exports various noise generation algorithms.
 */
namespace noise {
    /**
     * Adds salt and pepper noise to the input image.
     *
     * \param source The image to modify
     * \param density The probability of a pixel being either white or black
     */
    template<typename T>
    void salt_and_pepper(
        image<T> & source,
        float density);

    /**
     * Adds salt and pepper noise to a copy of the input image.
     *
     * \param source The image to modify
     * \param density The probability of a pixel being either white or black
     * \returns The modified image
     */
    template<typename T>
    image<T> salt_and_pepper(
        image<T> source,
        float density)
    {
        salt_and_pepper(source, density);
        return source;
    }

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
