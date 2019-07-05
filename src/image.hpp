#ifndef SPICE_IMAGE
#define SPICE_IMAGE

#import <vector>
#import <array>
#import <stdexcept>
#import <type_traits>
#import <limits>

#import <OpenImageIO/imageio.h>

/**
 * Contains the entire public interface of the library.
 */
namespace spice {
    /**
     * All the possible channel names.
     */
    enum class channel_names {
        RED, GREEN, BLUE,
        Y, Cb, Cr,
        U, V,
        CYAN, MAGENTA, YELLOW, BLACK,
        ALPHA
    };

    /**
     * Alias for convenience and forwards compatibility (channel_list might pack
     * much more information in the future: channel to index mapping etc...).
     */
    using channel_list = std::vector<channel_names>;

    /**
     * Describes a range of grey values of the particular type.
     */
    template<typename T>
    struct range {
        /** The low end of the range. */
        T min;
        /** The high end of the range. */
        T max;

        /**
         * Two ranges are equal if and only if both their min and their max are
         * equal as determined by comparison with `==`.
         */
        friend bool operator==(range<T> const & lhs, range<T> const & rhs)
        {
            if (lhs.min == rhs.min && lhs.max == rhs.max)
                return true;
            return false;
        }
    };

    /**
     * Refers to a single pixel in a `spice::image`. Note that this class has no
     * information about the size of the image it is referring to and thus
     * cannot perform bounds checking. Use `image::at` for that.
     */
    template<typename T>
    struct pixel_view {
    private:
        T * const m_data;
    public:
        /**
         * Constructs a `pixel_view` referring to the pixel starting at `data`.
         * Evidently the view is invalid if a `nullptr` is passed. Similarly
         * it is the user's responsibility to not refer to the pixel view after
         * the referred-to data has been invalidated.
         * It is therefore recommended to never use this structure explicitly
         * and instead only via `image::operator[]`.
         */
        constexpr pixel_view(T * const data = nullptr):
        m_data(data) {}
        /**
         * Copy constructor. Constructs a `pixel_view` referring to the same
         * data as the copied `pixel_view`
         *
         * \param other The `pixel_view` to copy
         */
        constexpr pixel_view(pixel_view const & other):
        m_data(other.m_data) {}

        /**
         * Allows accessing pixel data with subscript notation. No bounds
         * checking is performed (use `image::at` for this purpose).
         *
         * \note Accessing pixel values via `image::operator()` is slightly
         * faster since no intermediate objects have to be constructed.
         * Iterating over `image::data` is faster still.
         *
         * Calling `my_image[42][47][2]` on an RGB image would return the blue
         * channel value in column 43 (index 42), row 48 of the image.
         * 
         * \param channel The index of the channel to retrieve the data from
         * \returns A single, one-channel data point of the image
         */
        T& operator[](size_t channel)
        {
            return m_data[channel];
        }
        /**
         * Allows accessing pixel data with subscript notation. No bounds
         * checking is performed (use `image::at` for this purpose).
         *
         * \note Accessing pixel values via `image::operator()` is slightly
         * faster since no intermediate objects have to be constructed.
         * Iterating over `image::data` is faster still.
         *
         * Calling `my_image[42][47][2]` on an RGB image would return the blue
         * channel value in column 43 (index 42), row 48 of the image.
         * 
         * \param channel The index of the channel to retrieve the data from
         * \returns A single, one-channel data point of the image
         */
        T const & operator[](size_t channel) const
        {
            return m_data[channel];
        }
    };

    /**
     * Refers to a column of pixel data in a `spice::image`. Note that this
     * class has no information about the size of the image it is referring to
     * and thus cannot perform bounds checking. Use `image::at` for that.
     */
    template<typename T>
    struct column_view {
    private:
        T * const m_data;
    public:
        /**
         * Constructs a `column_view` referring to the column starting at `data`.
         * Evidently the view is invalid if a `nullptr` is passed. Similarly
         * it is the user's responsibility to not refer to the column view after
         * the referred-to data has been invalidated.
         * It is therefore recommended to never use this structure explicitly
         * and instead only via `image::operator[]`.
         */
        constexpr column_view(T * const data = nullptr);
        /**
         * Copy constructor. Constructs a `column_view` referring to the same
         * data as the copied `column_view`
         *
         * \param other The `column_view` to copy
         */
        constexpr column_view(column_view const & other);

        /**
         * Allows accessing pixel data with subscript notation. No bounds
         * checking is performed (use `image::at` for this purpose).
         *
         * \note Accessing pixel values via `image::operator()` is slightly
         * faster since no intermediate objects have to be constructed.
         * Iterating over `image::data` is faster still.
         *
         * Calling `my_image[42][47][2]` on an RGB image would return the blue
         * channel value in column 43 (index 42), row 48 of the image.
         * 
         * \param row The index of the row of the pixel to retrieve a view for
         * \returns A `pixel_view` referring to the indicated column's data
         */
        pixel_view<T> operator[](size_t row);
        /**
         * Allows accessing pixel data with subscript notation. No bounds
         * checking is performed (use `image::at` for this purpose).
         *
         * \note Accessing pixel values via `image::operator()` is slightly
         * faster since no intermediate objects have to be constructed.
         * Iterating over `image::data` is faster still.
         *
         * Calling `my_image[42][47][2]` on an RGB image would return the blue
         * channel value in column 43 (index 42), row 48 of the image.
         * 
         * \param row The index of the row of the pixel to retrieve a view for
         * \returns A `pixel_view` referring to the indicated column's data
         */
        const pixel_view<T> operator[](size_t row) const;
    };

    /**
     * spice's `image` class stores pixel data of arbitrary channel order and
     * arbitrary channel semantics in channel, column, row order.
     * Thus, a 3x2 pixel BGR image would be stored in an 18 element vector
     * where the first element represents the blue channel of the top left
     * pixel, the second the green, the third the red and the fourth element
     * would be the blue channel of the leftmost pixel of the second row.
     */
    template<typename T>
    class image {
    private:
        std::vector<T> m_data;
        size_t m_width;
        size_t m_height;
        channel_list m_channel_semantics;
    public:
        /**
         * Convenience constant defining the value representing black and the
         * value representing white. This will be `{ 0, 1 }` for floating point
         * types and the type's minumum and maximum for all others.
         *
         * Note that this range convention implies that floating point images
         * can contain pixel values beyond the black-white spectrum (meaning
         * values that will be clipped when rendered).
         */
        static const constexpr range intensity_range = ([]() -> range<T> {
            if (std::is_floating_point<T>::value) return {T{}, T(1.f)};
            return {
                std::numeric_limits<T>::min(),
                std::numeric_limits<T>::max()};
        })();

        /**
         * Creates an image and initialises it with the values found in the
         * passed data array.
         * \param width The width of the image
         * \param height The height of the image
         * \param channel_semantics The meaning to assign to the channels
         *
         * \todo Catch data of size != width * height * channel_semantics.size()
         */
        image(
            std::vector<T> data,
            size_t const width,
            size_t const height,
            channel_list const channel_semantics):
        m_data(data),
        m_width(width), m_height(height),
        m_channel_semantics(channel_semantics) {}
        /**
         * Creates an image and initialises it with black pixels.
         * \param width The width of the image
         * \param height The height of the image
         * \param channel_semantics The meaning to assign to the channels
         */
        image(
            size_t const width = 1,
            size_t const height = 1,
            channel_list const channel_semantics = {
                channel_names::RED,
                channel_names::GREEN,
                channel_names::BLUE
            }):
        m_data(width * height * channel_semantics.size(), T{}),
        m_width(width), m_height(height),
        m_channel_semantics(channel_semantics) {}
        /**
         * Copy constructor. Performs a deep copy of the passed image.
         * \param other The image to copy
         */
        image(image const & other):
        m_data(other.m_data),
        m_width(other.m_width), m_height(other.m_height),
        m_channel_semantics(other.m_channel_semantics) {}

        /**
         * The width of the image.
         */
        size_t width() const
        { return m_width; }
        /**
         * The height of the image.
         */
        size_t height() const
        { return m_height; }
        /**
         * The number of channels in the image.
         */
        size_t channels() const
        { return m_channel_semantics.size(); }

        /**
         * The meaning assigned to the individual channels.
         */
        channel_list const & channel_semantics() const
        { return m_channel_semantics; }

        /**
         * Accessor to the underlying vector for fast linear data manipulation.
         * \returns a reference to the raw image data
         */
        std::vector<T> & data()
        { return m_data; }
        /**
         * Accessor to the underlying vector for fast linear data manipulation.
         * \returns a const reference to the raw image data
         */
        const std::vector<T> & data() const
        { return m_data; }

        /**
         * Allows accessing pixel data with subscript notation. No bounds
         * checking is performed (use `image::at` for this purpose).
         *
         * \note Accessing pixel values via `image::operator()` is slightly
         * faster since no intermediate objects have to be constructed.
         * Iterating over `image::data` is faster still.
         *
         * Calling `my_image[42][47][2]` on an RGB image would return the blue
         * channel value in column 43 (index 42), row 48 of the image.
         * 
         * \param column The index of the column to retrieve a view for
         * \returns A `column_view` referring to the indicated column's data
         */
        column_view<T> operator[](size_t column);
        /**
         * Allows accessing pixel data with subscript notation. No bounds
         * checking is performed (use `image::at` for this purpose).
         *
         * \note Accessing pixel values via `image::operator()` is slightly
         * faster since no intermediate objects have to be constructed.
         * Iterating over `image::data` is faster still.
         *
         * Calling `my_image[42][47][2]` on an RGB image would return the blue
         * channel value in column 43 (index 42), row 48 of the image.
         * 
         * \param column The index of the column to retrieve a view for
         * \returns A `column_view` referring to the indicated column's data
         */
        const column_view<T> operator[](size_t column) const;

        /**
         * Retrieves a reference to the entire pixel located at the indicated
         * coordinates. No bounds checking is performed.
         *
         * \param column
         * \param row
         * \returns a `pixel_view` referring to the pixel at the indicated
         * location
         */
        pixel_view<T> operator()(size_t column, size_t row)
        {
            return pixel_view<T>(&m_data[
                column * m_height * channels() +    // x offset
                row * channels()]);                 // y offset
        }
        /**
         * Retrieves a reference to the entire pixel located at the indicated
         * coordinates. No bounds checking is performed.
         *
         * \param column
         * \param row
         * \returns a `pixel_view` referring to the pixel at the indicated
         * location
         */
        const pixel_view<const T> operator()(size_t column, size_t row) const
        {
            return pixel_view<const T>(&m_data[
                column * m_height * channels() +    // x offset
                row * channels()]);                 // y offset
        }
        /**
         * Retrieves a reference to a single channel's value located at the
         * indicated coordinates. No bounds checking is performed.
         *
         * \param column
         * \param row
         * \param channel
         * \returns The value at the indicated location
         */
        T & operator()(size_t column, size_t row, size_t channel)
        {
            return m_data[
                column * m_height * channels() +    // x offset
                row * channels() +                  // y offset
                channel];                           // channel offset
        }
        /**
         * Retrieves a reference to a single channel's value located at the
         * indicated coordinates. No bounds checking is performed.
         *
         * \param column
         * \param row
         * \param channel
         * \returns The value at the indicated location
         */
        T const & operator()(size_t column, size_t row, size_t channel) const
        {
            return m_data[
                column * m_height * channels() +    // x offset
                row * channels() +                  // y offset
                channel];                           // channel offset
        }

        /**
         * Retrieves a reference to the entire pixel located at the indicated
         * coordinates. Unlike the subscript and call operators, this accessor
         * function is bounds checked and will throw an exception if the
         * pixel is outside the image boundaries.
         *
         * \param column
         * \param row
         * \returns a `pixel_view` referring to the pixel at the indicated
         * location
         * \throws std::out_of_range
         */
        pixel_view<T> at(size_t column, size_t row);
        /**
         * Retrieves a reference to the entire pixel located at the indicated
         * coordinates. Unlike the subscript and call operators, this accessor
         * function is bounds checked and will throw an exception if the
         * pixel is outside the image boundaries.
         *
         * \param column
         * \param row
         * \returns a `pixel_view` referring to the pixel at the indicated
         * location
         * \throws std::out_of_range
         */
        const pixel_view<T> at(size_t column, size_t row) const;
        /**
         * Retrieves a reference to a single channel's value located at the
         * indicated coordinates. Unlike the subscript and call operators, this
         * accessor function is bounds checked and will throw an exception if
         * the pixel is outside the image boundaries.
         *
         * \param column
         * \param row
         * \param channel
         * \returns The value at the indicated location
         * \throws std::out_of_range
         */
        T & at(size_t column, size_t row, size_t channel);
        /**
         * Retrieves a reference to a single channel's value located at the
         * indicated coordinates. Unlike the subscript and call operators, this
         * accessor function is bounds checked and will throw an exception if
         * the pixel is outside the image boundaries.
         *
         * \param column
         * \param row
         * \param channel
         * \returns The value at the indicated location
         * \throws std::out_of_range
         */
        T const & at(size_t column, size_t row, size_t channel) const;

        /**
         * Compares two images with one another. Two images are considered to be
         * equal if they have the same `width`, `height`, `channel_semantics`
         * and contain the same `data`.
         */
        friend bool operator==(image<T> const & lhs, image<T> const & rhs)
        {
            if (lhs.m_width != rhs.m_width) return false;
            if (lhs.m_height != rhs.m_height) return false;
            if (lhs.m_channel_semantics != rhs.m_channel_semantics)
                return false;
            if (lhs.m_data != rhs.m_data) return false;
            return true;
        }

        /**
         * Compares two images with one another. `operator!=` is implemented as
         * the negation of `image::operator==`.
         */
        friend bool operator!=(image<T> const & lhs, image<T> const & rhs)
        { return !(lhs == rhs); }

        /**
         * Adds the first image element-wise to the second.
         * If the two images have different dimensions, only the overlapping
         * parts will be taken into account. The images will be aligned by their
         * top-left corners.
         */
        image<T>& operator+=(image<T> const & rhs);
        /**
         * Subtracts the first image element-wise from the second.
         * If the two images have different dimensions, only the overlapping
         * parts will be taken into account. The images will be aligned by their
         * top-left corners.
         */
        image<T>& operator-=(image<T> const & rhs);
        /**
         * Multiplies the first image element-wise with the second.
         * If the two images have different dimensions, only the overlapping
         * parts will be taken into account. The images will be aligned by their
         * top-left corners.
         */
        image<T>& operator*=(image<T> const & rhs);
        /**
         * Divides the first image element-wise from the second.
         * If the two images have different dimensions, only the overlapping
         * parts will be taken into account. The images will be aligned by their
         * top-left corners.
         */
        image<T>& operator/=(image<T> const & rhs);

        /**
         * Adds the two images element-wise.
         * If the two images have different dimensions, only the overlapping
         * parts will be taken into account. The images will be aligned by their
         * top-left corners.
         */
        friend image<T> operator+(image<T> lhs, image<T> const & rhs) {
            lhs += rhs;
            return lhs;
        }
        /**
         * Subtracts the two images element-wise.
         * If the two images have different dimensions, only the overlapping
         * parts will be taken into account. The images will be aligned by their
         * top-left corners.
         */
        friend image<T> operator-(image<T> lhs, image<T> const & rhs) {
            lhs -= rhs;
            return lhs;
        }
        /**
         * Multiplies the two images element-wise.
         * If the two images have different dimensions, only the overlapping
         * parts will be taken into account. The images will be aligned by their
         * top-left corners.
         */
        friend image<T> operator*(image<T> lhs, image<T> const & rhs) {
            lhs *= rhs;
            return lhs;
        }
        /**
         * Divides the two images element-wise.
         * If the two images have different dimensions, only the overlapping
         * parts will be taken into account. The images will be aligned by their
         * top-left corners.
         */
        friend image<T> operator/(image<T> lhs, image<T> const & rhs) {
            lhs /= rhs;
            return lhs;
        }
    };

    template<typename T>
    image<T> load_image(char const * filename)
    {
        auto file = OIIO::ImageInput::open(filename);
        // if (!file)
        // TODO: handle error

        const OIIO::ImageSpec & spec = file->spec();
        channel_list channels = spec.channelnames;

        std::vector<T> img_data(spec.width * spec.height * spec.nchannels);
        file->read_image(TypeDesc format, &img_data[0]);

        image<T> result(
            img_data,
            spec.width,
            spec.height,
            channels
            );

        file->close();
    }
}


#endif // SPICE_IMAGE
