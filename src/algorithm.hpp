#ifndef SPICE_ALGORITHM
#define SPICE_ALGORITHM

#import "image.hpp"

namespace spice {
    /**
     * Adds gaussian noise to an image.
     *
     * \param source The image to modify
     * \param sigma The standard deviation
     * \returns The modified image
     */
    template<typename T>
    image<T> & gaussian_noise(
        image<T> & source
        float const sigma);
}

#endif // SPICE_ALGORITHM
