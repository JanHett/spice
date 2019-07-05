/**
 * \file
 * This header defines a collection of blurring and averaging functions.
 */
#ifndef SPICE_BLUR
#define SPICE_BLUR

#import "image.hpp"

namespace spice {
/**
 * This namespace exports a collection of blurring and averaging functions.
 */
namespace blur {
    /**
     * Applies an approximation of a gaussian blur to the source image.
     * This function has an asymptotic time complexity of \f$O(w * h * c)\f$
     * where \f$w, h, c\f$ refer to the width, height and channel count of the
     * source image.
     *
     * \param source The image to blur
     * \param sigma The standard deviation ("size") of the blur
     * \param passes How many box-blur passes should be applied. A higher number
     * will yield a closer approximation at the cost of longer running time.
     * \returns A blurred copy of the input image
     *
     * \todo split treatment of channels and horizontal/vertical part into
     * individual threads
     */
    template<typename T>
    void fast_gaussian(image<T> & source, float sigma, size_t passes = 5);

    /**
     * Applies an approximation of a gaussian blur to a copy of the source
     * image.
     * This function has an asymptotic time complexity of \f$O(w * h * c)\f$
     * where \f$w, h, c\f$ refer to the width, height and channel count of the
     * source image.
     *
     * \param source The image to blur
     * \param sigma The standard deviation ("size") of the blur
     * \param passes How many box-blur passes should be applied. A higher number
     * will yield a closer approximation at the cost of longer running time.
     * \returns A blurred copy of the input image
     *
     * \todo split treatment of channels and horizontal/vertical part into
     * individual threads
     */
    template<typename T>
    image<T> fast_gaussian(image<T> source, float sigma, size_t passes = 5)
    {
        fast_gaussian(source, sigma, passes);
        return source;
    }
}
}

#endif // SPICE_BLUR
