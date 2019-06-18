#ifndef SPICE_IMAGE
#define SPICE_IMAGE

#import <memory>
#import <tuple>

namespace spice {
    enum class channel_order {
        RGB,
        RGBA,
        BGR,
        BGRA,
        CMYK,
        CMYKA
    };

    enum class channels {
        R, G, B,
        C, M, Y, K,
        A,
    };

    /// Width, height and channel count of an image
    using image_shape = std::tuple<size_t, size_t, channel_order>;

    /// Reference to a pixel value in an image
    class pixel_view {
    private:
        float * const m_data;
        size_t m_channels;
        bool m_valid = true;
    public:
        pixel_view(float * const data, channel_order channels);

        /// The channel information of the image
        channel_order & channels();
        channel_order channels() const;

        /// Returns false if the pixel is outside the image dimensions,
        /// true otherwise.
        operator bool();
    };

    /// Reference to a column in an image.
    /// Use this sparingly as it tends to not be very cache-friendly.
    class column_view {
    private:
        float * const m_data;
        size_t m_height;
        size_t m_line_length;
        size_t m_channels;
        bool m_valid = true;
    public:
        column_view(
            float * const data,
            size_t line_length,
            size_t height,
            channel_order channels);

        pixel_view operator[] (size_t line);

        /// Returns false if the column is outside the image dimensions,
        /// true otherwise.
        operator bool();
    };

    class image {
    private:
        std::unique_ptr<float> m_data;
        image_shape m_shape;
    public:
        image();
        /// Create an image filled with black
        image(size_t width, size_t height, channel_order channels);
        image(image const & other);
        image(float const * const data,
            size_t width,
            size_t height,
            channel_order channels);
        image(uint16_t const * const data,
            size_t width,
            size_t height,
            channel_order channels);
        image(uint8_t const * const data,
            size_t width,
            size_t height,
            channel_order channels);

        /// The width, height and channel count of the image
        image_shape & shape();
        image_shape const & shape() const;
        /// The width of the image
        size_t & width();
        size_t width() const;
        /// The height of the image
        size_t & height();
        size_t height() const;
        /// The channel information of the image
        channel_order & channels();
        channel_order channels() const;

        /// The raw data organised linewise in RGB(A)/CMYK(A) format.
        /// Top-left pixel is at index 0.
        float * const data();
        float * const data() const;

        /// Get the pixel at the provided coordinates
        pixel_view operator() (size_t x, size_t y);

        /// Get the specified column
        column_view operator[] (size_t column);
    };
}

#endif // SPICE_IMAGE
