/**
 * \file
 * This header defines  utilities to print some of spice's types to the console.
 */
#ifndef SPICE_PRINT
#define SPICE_PRINT

#include <vector>
#include <cmath>

#include "image.hpp"

namespace spice {
/**
 * This namespace exports utilities to print some of spice's types to the
 * console.
 * These are mainly meant for debugging purposes.
 * \note Functions that print in colour assume that the terminal supports 24 bit
 * colour output.
 */
namespace print {
    /**
     * Insert escape codes for terminals supporting 8 bpc colours into a string.
     *
     * \param str The string to surround with escape codes
     * \param foreground The text colour
     * \param background The background colour
     */
    template<typename T_data = float, typename T_color = color<T_data>>
    std::string color_escape_string(
        std::string const & str,
        T_color const & foreground,
        T_color const & background)
    {
        const float scaling_factor = 255 / (image<T_data>::intensity_range.max -
            image<T_data>::intensity_range.min);
        const float offset = 0 - image<T_data>::intensity_range.min;

        color<T_data> bg_scaled = background;
        bg_scaled *= scaling_factor + offset;
        color<T_data> fg_scaled = foreground;
        fg_scaled *= scaling_factor + offset;

        // std::cout << "\nScale: " << scaling_factor << ", offset " << offset << "\n";
        // std::cout << "Vector size: " << bg_scaled.size() << "\n";
        // std::cout << "Original |" << background[0] << "|" << background[1] << "|" << background[2] << "\n";
        // std::cout << "Scaled   |" << bg_scaled[0] << "|" << bg_scaled[1] << "|" << bg_scaled[2] << "\n";

        return std::string("\033[48;2;") +
        // set BG-colours
        std::to_string(static_cast<int>(std::floor(bg_scaled[0]))) + ";" +
        std::to_string(static_cast<int>(std::floor(bg_scaled[1]))) + ";" +
        std::to_string(static_cast<int>(std::floor(bg_scaled[2]))) +
        // set FG-colours
        ";38;2;" +
        std::to_string(static_cast<int>(std::floor(fg_scaled[0]))) + ";" +
        std::to_string(static_cast<int>(std::floor(fg_scaled[1]))) + ";" +
        std::to_string(static_cast<int>(std::floor(fg_scaled[2]))) + "m" +
        // add the actual string and the ending escape code
        str + "\033[0m";
    }

    /**
     * Prints an image in pixel-art style to the `stream`.
     *
     * \note Since there is no way to reliably make exactly square pixels, the
     * aspect ratio will be off.
     */
    template<typename T>
    void image(image<T> const & img, size_t stride = 1,
        std::ostream & stream = std::cout) {
        for (size_t x = 0; x < img.height(); x += stride){
            for (size_t y = 0; y < img.width(); y += stride)
                stream << print::color_escape_string<float,
                    const pixel_view<float>>(
                    "  ", img(y, x), img(y, x));
            stream << "\n";
        }
    }

    /**
     * Prints a histogram to the provided stream.
     *
     * \note You should use a color type that allows overflow to avoid clipping
     * while calculating the color for the individual bars.
     *
     * \param histogram The data to print
     * \param max_amplitude The length of the longest bar in single spaces
     * \param colors The colour to print each channel in. Note that the colours
     * should be represented as RGB, superfluous colours will be ignored and
     * missing colours will be represented as grey.
     * \param stream The stream to insert the histogram into
     */
    template<typename T = float>
    void histogram(std::vector<std::vector<size_t>> const & histogram,
        size_t max_amplitude,
        std::vector<color<T>> colors,
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
        const float scale = static_cast<float>(max_amplitude) / max_sample;

        // do the actual printing
        for (size_t sample = 0; sample < histogram[0].size(); ++sample) {
            for (size_t cell = 0; cell < max_amplitude; ++cell) {
                // calculate colour (sum of all active channels' colours divided
                // by number of channels) and print
                color<T> cell_color({0,0,0});
                for (size_t channel = 0; channel < histogram.size(); ++channel)
                {
                    if ((histogram[channel][sample] * scale) < cell) {
                        if (colors.size() > channel)
                            cell_color += colors[channel];
                        else
                            cell_color += color<T>({0.5, 0.5, 0.5});
                    }
                }
                // normalise colours
                cell_color /= std::max({cell_color[0],
                    cell_color[1],
                    cell_color[2]});
                cell_color = color<T>({1, 1, 1}) - cell_color;
                stream << color_escape_string(" ", cell_color, cell_color);
            }
            stream << "\n";
        }
    }
}
}

#endif // SPICE_PRINT
