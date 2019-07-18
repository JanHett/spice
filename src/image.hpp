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
namespace spice
{
    //--------------------------------------------------------------------------
    // Utility structs and typedefs
    //--------------------------------------------------------------------------

    /**
     * Alias for convenience and forwards compatibility (channel_list might pack
     * much more information in the future: channel to index mapping etc...).
     */
    using channel_list = std::vector<std::string>;

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
            return lhs.min == rhs.min && lhs.max == rhs.max;
        }
    };

    //--------------------------------------------------------------------------
    // Operator overloads for vector-like types
    //--------------------------------------------------------------------------

    /**
     * Compares two vector-like types (e.g. `spice::color` or
     * `spice::pixel_view`).
     * Two vector-like types are equal if they are of the same size and all of
     * their elements are equal as determined by comparison with `==`.
     */
    template<typename T_lhs, typename T_rhs>
    bool operator==(
        T_lhs const & lhs, T_rhs const & rhs)
    {
        if (!(lhs.size() == rhs.size()))
            return false;
        for (size_t idx = 0; idx < lhs.size(); ++idx)
            if (!(lhs[idx] == rhs[idx]))
                return false;
        return true;
    }

    /**
     * Compares two vector-like types (e.g. `spice::color` or
     * `spice::pixel_view`).
     * Two vector-like types are not equal if they are either of different sizes
     * or any of their elements are not equal as determined by comparison with
     * `==`.
     */
    template<typename T_lhs, typename T_rhs>
    bool operator!=(
        T_lhs const & lhs, T_rhs const & rhs)
    { return !(lhs == rhs); }

    /**
     * Adds two vector-like types component-wise. The vectors are assumed to be
     * of equal size - no bounds checking is performed.
     */
    template<typename T_lhs, typename T_rhs>
    std::enable_if_t<!std::is_arithmetic<T_rhs>::value, T_lhs&> operator+=(
        T_lhs & lhs, T_rhs const & rhs)
    {
        for (size_t idx = 0; idx < lhs.size(); ++idx)
            lhs[idx] += rhs[idx];
        return lhs;
    }

    /**
     * Adds an arithmetic type to a vector-like type.
     */
    template<typename T_lhs, typename T_rhs>
    std::enable_if_t<std::is_arithmetic<T_rhs>::value, T_lhs&> operator+=(
        T_lhs & lhs, T_rhs const & rhs)
    {
        for (size_t idx = 0; idx < lhs.size(); ++idx)
            lhs[idx] += rhs;
        return lhs;
    }

    /**
     * Adds two vector-like types component-wise. The vectors are assumed to be
     * of equal size - no bounds checking is performed.
     */
    template<typename T_lhs, typename T_rhs>
    T_lhs operator+(T_lhs lhs, T_rhs const & rhs)
    {
        lhs += rhs;
        return lhs;
    }

    /**
     * Subtracts two vector-like types component-wise. The vectors are assumed
     * to be of equal size - no bounds checking is performed.
     */
    template<typename T_lhs, typename T_rhs>
    std::enable_if_t<!std::is_arithmetic<T_rhs>::value, T_lhs&> operator-=(
        T_lhs & lhs, T_rhs const & rhs)
    {
        for (size_t idx = 0; idx < lhs.size(); ++idx)
            lhs[idx] -= rhs[idx];
        return lhs;
    }

    /**
     * Subtracts an arithmetic type from a vector-like type.
     */
    template<typename T_lhs, typename T_rhs>
    std::enable_if_t<std::is_arithmetic<T_rhs>::value, T_lhs&> operator-=(
        T_lhs & lhs, T_rhs const & rhs)
    {
        for (size_t idx = 0; idx < lhs.size(); ++idx)
            lhs[idx] -= rhs;
        return lhs;
    }

    /**
     * Subtracts two vector-like types component-wise. The vectors are assumed
     * to be of equal size - no bounds checking is performed.
     */
    template<typename T_lhs, typename T_rhs>
    T_lhs operator-(T_lhs lhs, T_rhs const & rhs)
    {
        lhs -= rhs;
        return lhs;
    }

    /**
     * Multiplies two vector-like types component-wise. The vectors are assumed
     * to be of equal size - no bounds checking is performed.
     */
    template<typename T_lhs, typename T_rhs>
    std::enable_if_t<!std::is_arithmetic<T_rhs>::value, T_lhs&> operator*=(
        T_lhs & lhs, T_rhs const & rhs)
    {
        for (size_t idx = 0; idx < lhs.size(); ++idx)
            lhs[idx] *= rhs[idx];
        return lhs;
    }

    /**
     * Multiplies an arithmetic type with a vector-like type.
     */
    template<typename T_lhs, typename T_rhs>
    std::enable_if_t<std::is_arithmetic<T_rhs>::value, T_lhs&> operator*=(
        T_lhs & lhs, T_rhs const & rhs)
    {
        for (size_t idx = 0; idx < lhs.size(); ++idx)
            lhs[idx] *= rhs;
        return lhs;
    }

    /**
     * Multiplies two vector-like types component-wise. The vectors are assumed
     * to be of equal size - no bounds checking is performed.
     */
    template<typename T_lhs, typename T_rhs>
    T_lhs operator*(T_lhs lhs, T_rhs const & rhs)
    {
        lhs *= rhs;
        return lhs;
    }

    /**
     * Divides two vector-like types component-wise. The vectors are assumed to
     * be of equal size - no bounds checking is performed.
     */
    template<typename T_lhs, typename T_rhs>
    std::enable_if_t<!std::is_arithmetic<T_rhs>::value, T_lhs&> operator/=(
        T_lhs & lhs, T_rhs const & rhs)
    {
        for (size_t idx = 0; idx < lhs.size(); ++idx)
            lhs[idx] /= rhs[idx];
        return lhs;
    }

    /**
     * Divides a vector-like type by a primitive.
     */
    template<typename T_lhs, typename T_rhs>
    std::enable_if_t<std::is_arithmetic<T_rhs>::value, T_lhs&> operator/=(
        T_lhs & lhs, T_rhs const & rhs)
    {
        for (size_t idx = 0; idx < lhs.size(); ++idx)
            lhs[idx] /= rhs;
        return lhs;
    }

    /**
     * Divides two vector-like types component-wise. The vectors are assumed to
     * be of equal size - no bounds checking is performed.
     */
    template<typename T_lhs, typename T_rhs>
    T_lhs operator/(T_lhs lhs, T_rhs const & rhs)
    {
        lhs /= rhs;
        return lhs;
    }

    //--------------------------------------------------------------------------
    // High-level data structures
    //--------------------------------------------------------------------------

    // forward declaration to allow color constructor taking pixel_view argument
    template<typename T>
    struct pixel_view;

    /**
     * Represents a color. Is considered _vector-like_.
     */
    template<typename T = float>
    struct color {
    private:
        std::vector<T> m_data;
    public:

        /**
         * Constructs a color vector with zero elements and no semantics.
         */
        color(): m_data() {}
        /**
         * Constructs a color vector out of the passed list of values.
         */
        color(std::vector<T> data): m_data(data) {}
        /**
         * Constructs a color from a pixel_view, copying the values referenced
         * by the view.
         *
         * \note The type referenced by the `pixel_view` may differ from the
         * type of the `color` as long as the former is convertible to the
         * latter.
         *
         * \note Semantics are assumed to be RGB(A) for the first three or four
         * channels and unknown for the rest.
         */
        template<typename T_pv,
        std::enable_if_t<std::is_convertible<T_pv, T>::value, int> = 0>
        color(pixel_view<T_pv> const & view): m_data(view.size()) {
            for (size_t idx = 0; idx < m_data.size(); ++idx)
                m_data[idx] = view[idx];
        }
        /**
         * Constructs a color object from a hex code. 3, 4, 6 and 8 character
         * codes are supported. Semantics are automatically assumed to be
         * RGB(A).
         */
        // color(std::string hex):
        // m_data(hex.size() > 4 ? hex.size() / 2 : hex.size()) { /* TODO */ }

        /**
         * /returns The size of the color vector
         */
        [[nodiscard]] size_t size() const {
            return m_data.size();
        }

        /**
         * Copies values referenced by `other` to this color object.
         *
         * \note This will adjust the size of the color object to equal that of
         * the pixel view.
         */
        // color<T>& operator=(pixel_view<T> const & other)
        // {
        //     m_data.resize(other.size());
        //     for (size_t idx = 0; idx < size(); ++idx)
        //         (*this)[idx] = other[idx];
        //     return *this;
        // }

        /**
         * Returns a channel from this color vector with the provided offeset
         */
        T & operator[] (size_t channel) {
            return m_data[channel];
        }
        /**
         * Returns a channel from this color vector with the provided offeset
         */
        T const & operator[] (size_t channel) const {
            return m_data[channel];
        }
    };

    template<typename T>
    std::ostream& operator<<(std::ostream& os, color<T> const & pxl)
    {
        os << "(";
        for (size_t idx = 0; idx < pxl.size() - 1; ++idx)
            os << pxl[idx] << ", ";
        os << pxl[pxl.size() - 1];
        os << ")";
        return os;
    }

    /**
     * Refers to a single pixel in a `spice::image`. Note that this class has no
     * information about the size of the image it is referring to and thus
     * cannot perform bounds checking. Use `image::at` for that.
     * Fulfills concept _vector-like_.
     */
    template<typename T>
    struct pixel_view {
    private:
        T * const m_data;
        size_t m_nchannels;
    public:
        /**
         * Constructs a `pixel_view` referring to the pixel starting at `data`.
         * Evidently the view is invalid if a `nullptr` is passed. Similarly
         * it is the user's responsibility to not refer to the pixel view after
         * the referred-to data has been invalidated.
         * It is therefore recommended to never use this structure explicitly
         * and instead only via `image::operator[]`.
         *
         * \param data Pointer to the first channel of this pixel's data
         * \param channels The number of channels in this pixel
         */
        constexpr pixel_view(T * const data = nullptr, size_t channels = 0):
        m_data(data), m_nchannels(channels) {}
        /**
         * Copy constructor. Constructs a `pixel_view` referring to the same
         * data as the copied `pixel_view`
         *
         * \param other The `pixel_view` to copy
         */
        constexpr pixel_view(pixel_view const & other):
        m_data(other.m_data), m_nchannels(other.m_nchannels) {}

        /**
         * /returns The number of channels of the pixel
         */
        size_t size() const {
            return m_nchannels;
        }

        /**
         * Copies values referenced by `other` to data array referenced by this
         * `pixel_view`.
         *
         * \note This will copy the first _n_ channels, where _n =
         * min(this->size, other.size())_.
         */
        template<typename T_other>
        pixel_view<T>& operator=(pixel_view<T_other> const & other)
        {
            auto channels_to_copy = std::min(m_nchannels, other.size());
            for (size_t idx = 0; idx < channels_to_copy; ++idx)
                m_data[idx] = other[idx];
            return *this;
        }

        /**
         * Copies values referenced by `other` to data array referenced by this
         * `pixel_view`.
         *
         * \note This will copy the first _n_ channels, where _n =
         * min(this->size, other.size())_.
         */
        template<typename T_other>
        pixel_view<T>& operator=(color<T_other> const & other)
        {
            auto channels_to_copy = std::min(m_nchannels, other.size());
            for (size_t idx = 0; idx < channels_to_copy; ++idx)
                m_data[idx] = other[idx];
            return *this;
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

    template<typename T>
    std::ostream& operator<<(std::ostream& os, pixel_view<T> const & pxl)
    {
        os << "(";
        for (size_t idx = 0; idx < pxl.size() - 1; ++idx)
            os << pxl[idx] << ", ";
        os << pxl[pxl.size() - 1];
        os << ")";
        return os;
    }

    /**
     * Refers to a column of pixel data in a `spice::image`. Note that this
     * class has no information about the size of the image it is referring to
     * and thus cannot perform bounds checking. Use `image::at` for that.
     */
    template<typename T>
    struct column_view {
    private:
        T * const m_data;
        size_t m_nchannels;
    public:
        /**
         * Constructs a `column_view` referring to the column starting at `data`.
         * Evidently the view is invalid if a `nullptr` is passed. Similarly
         * it is the user's responsibility to not refer to the column view after
         * the referred-to data has been invalidated.
         * It is therefore recommended to never use this structure explicitly
         * and instead only via `image::operator[]`.
         *
         * \param data Pointer to the first channel of this column's data
         * \param channels The number of channels in a pixel
         */
        constexpr column_view(T * const data = nullptr, size_t channels = 0):
        m_data(data), m_nchannels(channels) {}
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
        pixel_view<T> operator[](size_t row)
        {
            return pixel_view<T>(&m_data[row * m_nchannels], m_nchannels);
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
         * \param row The index of the row of the pixel to retrieve a view for
         * \returns A `pixel_view` referring to the indicated column's data
         */
        pixel_view<T const> operator[](size_t row) const
        {
            return pixel_view<T>(&m_data[row * m_nchannels], m_nchannels);
        }
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
         * types and the type's minimum and maximum for all others.
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
         * \note The `data` vector should be structured in column-major order.
         *
         * \param data The raw image data
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
            channel_list const & channel_semantics):
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
            size_t const width = 0,
            size_t const height = 0,
            channel_list const & channel_semantics = {}):
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
        [[nodiscard]] size_t width() const
        { return m_width; }
        /**
         * The height of the image.
         */
        [[nodiscard]] size_t height() const
        { return m_height; }
        /**
         * The number of channels in the image.
         */
        [[nodiscard]] size_t channels() const
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
        column_view<T> operator[](size_t column)
        {
            return column_view<T>(&m_data[column * m_height * channels()],
                channels());
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
         * \param column The index of the column to retrieve a view for
         * \returns A `column_view` referring to the indicated column's data
         */
        const column_view<T> operator[](size_t column) const
        {
            return column_view<T>(&m_data[column * m_height * channels()],
                channels());
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
        pixel_view<T> operator()(size_t column, size_t row)
        {
            return pixel_view<T>(&m_data[
                column * m_height * channels() +    // x offset
                row * channels()],                  // y offset
                channels());
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
        const pixel_view<T const> operator()(size_t column, size_t row) const
        {
            return pixel_view<T const>(&m_data[
                column * m_height * channels() +    // x offset
                row * channels()],                  // y offset
                channels());
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

        image<T> transpose() const {
            image<T> new_i(m_height, m_width, m_channel_semantics);

            for (size_t x = 0; x < m_width; ++x)
                for (size_t y = 0; y < m_height; ++y)
                    new_i(y, x) = (*this)(x, y);

            return new_i;
        }
    };

    /**
     * This namespace contains utility functionality like adapters between
     * interfaces.
     */
    namespace helpers
    {
        /**
         * Wraps `std::enable_if_t` in such a way that the template is enabled
         * when the first two template arguments are equal as determined by
         * `std::is_same`. When enabled, this type becomes equivalent to the
         * third argument `T_result`.
         */
        template<typename T_lhs, typename T_rhs, typename T_result>
        using enable_if_same_t = std::enable_if_t<std::is_same<
            T_lhs,
            T_rhs>::value,
        T_result>;

        /** Converts a C++ double to OIIO::TypeDesc::DOUBLE. */
        template<typename T>
        constexpr enable_if_same_t<T, double, OIIO::TypeDesc>
        type_to_typedesc()
        {
            return OIIO::TypeDesc::DOUBLE;
        }
        /** Converts a C++ float to OIIO::TypeDesc::FLOAT. */
        template<typename T>
        constexpr enable_if_same_t<T, float, OIIO::TypeDesc>
        type_to_typedesc()
        {
            return OIIO::TypeDesc::FLOAT;
        }
        /** Converts a C++ uint32_t to OIIO::TypeDesc::UINT. */
        template<typename T>
        constexpr enable_if_same_t<T, uint32_t, OIIO::TypeDesc>
        type_to_typedesc()
        {
            return OIIO::TypeDesc::UINT;
        }
        /** Converts a C++ uint16_t to OIIO::TypeDesc::UINT16. */
        template<typename T>
        constexpr enable_if_same_t<T, uint16_t, OIIO::TypeDesc>
        type_to_typedesc()
        {
            return OIIO::TypeDesc::UINT16;
        }
        /** Converts a C++ uint8_t to OIIO::TypeDesc::UINT8. */
        template<typename T>
        constexpr enable_if_same_t<T, uint8_t, OIIO::TypeDesc>
        type_to_typedesc()
        {
            return OIIO::TypeDesc::UINT8;
        }
    }

    /**
     * Loads an image from disk and returns a representation of the indicated
     * data type. All conversions are handled internally by OIIO.
     * If the image could not be read, an empty image is returned (size 0x0x0).
     *
     * \param filename The path on disk relative to the current working
     * directory
     * \returns An image object representing the file contents
     */
    template<typename T>
    image<T> load_image(char const * filename)
    {
        auto file = OIIO::ImageInput::open(filename);
        // TODO more expressive error handling
        if (!file)
            return image<T>();

        const OIIO::ImageSpec & spec = file->spec();
        channel_list channels = spec.channelnames;

        std::vector<T> img_data(spec.width * spec.height * spec.nchannels);
        file->read_image(helpers::type_to_typedesc<T>(), &img_data[0]);

        // transpose the data before constructing an image object
        std::vector<T> tx_img_data(spec.width * spec.height * spec.nchannels);
        for (size_t y = 0; y < spec.height; ++y)
            for (size_t x = 0; x < spec.width; ++x)
                for (size_t chan = 0; chan < spec.nchannels; ++chan)
                    tx_img_data[
                        x * spec.height * spec.nchannels +
                        y * spec.nchannels +
                        chan
                    ] =
                    img_data[
                        y * spec.width * spec.nchannels +
                        x * spec.nchannels +
                        chan
                    ];

        image<T> result(
            tx_img_data,
            spec.width,
            spec.height,
            channels
            );

        file->close();

        return result;
    }

    /**
     * Writes an image to a file at the specified location.
     *
     * \param filename The path to write to relative to the current working
     * directory
     * \param data The image to save to disk
     * \param format The data format the file should be written with
     * \returns `true` if the image was successfully written, `false` if an
     * error occurred.
     */
    template<typename T>
    bool write_image(
        char const * filename,
        image<T> const & data,
        OIIO::TypeDesc const & format = helpers::type_to_typedesc<T>())
    {
        std::unique_ptr<OIIO::ImageOutput> out =
            OIIO::ImageOutput::create(filename);
        if (!out)
            return false;

        OIIO::ImageSpec spec(
            data.width(),
            data.height(),
            data.channels(),
            format);
        out->open(filename, spec);
        out->write_image(helpers::type_to_typedesc<T>(),
            &data.transpose().data()[0]); // transposing image before writing
        out->close();

        return true;
    }
}


#endif // SPICE_IMAGE
