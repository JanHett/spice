#ifndef SPICE_IMAGE
#define SPICE_IMAGE

#import <memory>
#import <tuple>

namespace spice {
    /// Width, height and channel count of an image
    using image_shape = std::tuple<size_t, size_t, uint8_t>;

    /// Reference to a pixel value in an image
    class pixel_view {
    private:
        float * const m_data;
        size_t m_channels;
        bool m_valid = true;
    public:
        pixel_view(float * const data, uint8_t channels);

        /// The channel count of the image
        uint8_t & channels();
        uint8_t channels() const;

        /// Returns false if the pixel is outside the image dimensions,
        /// true otherwise.
        operator bool();
    };

    /// Reference to a column in an image
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
            uint8_t channels);

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
        image(size_t width, size_t height, uint8_t channels);
        image(image const & other);

        /// The width, height and channel count of the image
        image_shape & shape();
        image_shape const & shape() const;
        /// The width of the image
        size_t & width();
        size_t width() const;
        /// The height of the image
        size_t & height();
        size_t height() const;
        /// The channel count of the image
        uint8_t & channels();
        uint8_t channels() const;

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
