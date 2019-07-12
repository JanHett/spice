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
    namespace
    {
        /**
         * creates a vector of box-blur sizes for a given standard deviation
         * `sigma`.
         * increasing n will approximate a true gaussian blur better but
         * decrease performance.
         */
        // TODO: get the box size right so a small sigma doesn't result in a
        // massive blur radius
        std::vector<size_t> fast_gaussian_box_sizes(float sigma, size_t n)
        {
            float w_ideal = std::sqrt((12 * sigma * sigma / n) + 1);
            float wl = std::floorf(w_ideal);
            if (static_cast<int>(wl) % 2 == 0) --wl;
            float wu = wl + 2;

            float m_ideal = (
                    12 * sigma * sigma -
                    n * wl * wl -
                    4 * n * wl -
                    3 * n
                ) / (
                    -4 * wl - 4
                );
            float m = std::roundf(m_ideal);

            std::vector<size_t> sizes(n);
            for (unsigned int i = 0; i < n; ++i)
                sizes[i] = std::round(i < m ? wl : wu);
            return sizes;
        }

        /**
         * Calculates a single pass of vertical box blur in-place.
         */
        template<typename T>
        void fast_gaussian_vertical_box_blur(image<T> & img,
            size_t radius)
        {
            // make sure the radius is not so large as to exceed the image's
            // dimensions
            radius = std::min(img.height(), radius);
            auto diameter = radius * 2 + 1;
            for (size_t column = 0; column < img.width(); ++column)
            {
                auto col = img[column];
                // accumulator initialised with vector of min-values
                color<T> accumulator(std::vector<T>(img.channels(),
                    image<T>::intensity_range.min));

                // calculate initial average
                for (size_t offset = 0; offset < radius; ++offset)
                    accumulator += col[offset];

                accumulator /= diameter;
                col[0] = accumulator;

                // calculate following pixel's values by subtracting left-most
                // pixel within the radius and adding the one to the right of
                // the right-most for each of the remaining pixels of the row.
                for (size_t row = 1; row < img.height(); ++row)
                {
                    accumulator -=
                    (col[std::max(0ll, static_cast<long long>(row - radius))] /
                        diameter) +
                    (col[std::min(img.height(), row + radius)] / diameter);
                    col[row] = accumulator;
                }
            }
        }
    }

    /**
     * Applies an approximation of a gaussian blur to the source image.
     * This function has an asymptotic time complexity of _O(w * h * c)_
     * where _w, h, c_ refer to the width, height and channel count of the
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
    void fast_gaussian(image<T> & source, float sigma, size_t passes = 5)
    {
        // algorithm described here:
        // http://blog.ivank.net/fastest-gaussian-blur.html (MIT license)

        const std::vector<size_t> radii =
            fast_gaussian_box_sizes(sigma, passes);
        for (size_t radius_i = 0; radius_i < radii.size(); ++radius_i) {
            fast_gaussian_vertical_box_blur(source, radii[radius_i]);
        }

        // blur the other way
        source = source.transpose();
        for (size_t radius_i = 0; radius_i < radii.size(); ++radius_i) {
            fast_gaussian_vertical_box_blur(source, radii[radius_i]);
        }

        source = source.transpose();
    }

    /**
     * Applies an approximation of a gaussian blur to a copy of the source
     * image.
     * This function has an asymptotic time complexity of _O(w * h * c)_
     * where _w, h, c_ refer to the width, height and channel count of the
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
    // template<typename T>
    // image<T> fast_gaussian(image<T> source, float sigma, size_t passes = 5)
    // {
    //     fast_gaussian(source, sigma, passes);
    //     return source;
    // }
}
}

#endif // SPICE_BLUR
