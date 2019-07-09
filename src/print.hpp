/**
 * \file
 * This header defines  utilities to print some of spice's types to the console.
 */
#ifndef SPICE_STATISTICS
#define SPICE_STATISTICS

#import <vector>
#import <cmath>

#import "image.hpp"

namespace spice {
/**
 * This namespace exports utilities to print some of spice's types to the
 * console.
 */
namespace print {
    /**
     * Prints a histogram to the provided stream.
     *
     * \param histogram The data to print
     * \param max_amplitude The length of the longest bar in single spaces
     * \param colors The colour to print each channel in. Note that superfluous
     * colours will be ignored and missing colours will be represented as grey.
     * \param stream The stream to insert the histogram into
     */
    void histogram(std::vector<std::vector<size_t>> const & histogram,
        size_t max_amplitude,
        std::vector<std::string> colors,
        std::ostream & stream = std::cout)
    {
        // find the most common value class per channel
        std::vector<size_t> max_samples(histogram.size());
        for (size_t chan = 0; chan < histogram.size(); ++chan)
            max_samples[chan] = *std::max_element(
                std::begin(histogram[chan]), std::end(histogram[chan]));

        // find the most common value class overall
        size_t max_sample = *std::max_element(
            std::begin(max_samples), std::end(max_samples));

        // calculate the scaling factor
        float scale = max_amplitude / max_sample;

        // do the actual printing
        for (size_t sample = 0; sample < histogram[0].size(); ++sample) {
            for (size_t cell = 0; cell < max_amplitude; ++cell) {
                for (size_t channel = 0; channel < histogram.size(); ++channel)
                {
                    // TODO: calculate colour (sum of all channels' colours
                    // divided by number of channels) and print
                }
            }
            std::cout << "\n";
        }
    }
}
}

#endif // SPICE_STATISTICS
