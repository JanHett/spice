/**
 * \file
 * This header defines a collection of statistical algorithms to analyse
 * images.
 */
#ifndef SPICE_STATISTICS
#define SPICE_STATISTICS

#import <vector>
#import <cmath>

#import "image.hpp"

namespace spice {
/**
 * This namespace exports a number of functions providing statistics on images.
 */
namespace statistics {
    /**
     * Calculates the source image's histogram. The resulting histogram will
     * contain the absolute count of pixel values matching each class.
     *
     * \param source The image to analyse
     * \param samples The granularity with which to divide the intensity range
     * of the image.
     * \returns A vector of vectors of length == `samples` where each of the
     * sub-vectors represents the histogram of one channel.
     */
    template<typename T>
    std::vector<std::vector<size_t>> histogram(
        image<T> const & source,
        size_t samples)
    {
        auto source_channels = source.channels();
        // create the empty histogram
        std::vector<std::vector<size_t>> hist;
        for (size_t chan = 0; chan < source_channels; ++chan)
            hist.push_back(std::vector<size_t>(samples, 0));

        // count
        auto data_length = source.width() * source.height();
        for (
            size_t offset = 0;
            offset < data_length;
            offset += source_channels)
        {
            for (size_t chan = 0; chan < source_channels; ++chan)
            {
                // calculate which class the pixel's channel value belongs to...
                size_t intensity_class = std::lround(
                    (source.data()[offset * source_channels + chan] /
                    image<T>::intensity_range.max) * (samples - 1));
                // and increment the relevant class' counter
                ++hist[chan][intensity_class];
            }
        }

        return hist;
    }
}
}

#endif // SPICE_STATISTICS
