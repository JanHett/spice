#ifndef SPICE_IMAGE
#define SPICE_IMAGE

#import <vector>
#import <stdexcept>

/**
 * Contains the entire public interface of the library.
 */
namespace spice {
    /**
     * \enum channel_semantics
     * `channel_semantics` lists all the colour spaces spice can work with.
     * Every base can be paired with an alpha channel by ways of the bit-wise
     * `&` operator. E.g. RGBA might be obtained like this:
     * `channel_semantics::RGB & channel_semantics::A`.
     * Note that different colour spaces cannot be combined with one another and
     * doing so results in undefined behaviour.
     */
    enum class channel_semantics {
        A      = 1,
        RGB    = 2,
        BGR    = 4,
        CMYK   = 6,
        YUV    = 8,
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
        constexpr pixel_view(T * const data = nullptr);
        /**
         * Copy constructor. Constructs a `pixel_view` referring to the same
         * data as the copied `pixel_view`
         *
         * \param other The `pixel_view` to copy
         */
        constexpr pixel_view(pixel_view const & other);

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
        T& operator[](size_t channel);
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
        T const & operator[](size_t channel) const;
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
        channel_semantics m_channel_semantics;
    public:
        /**
         * Creates an image and initialises it with black pixels.
         * \param width The width of the image
         * \param height The height of the image
         * \param channels The meaning to assign to the channels
         */
        image(
            size_t width = 1,
            size_t height = 1,
            channel_semantics channels = channel_semantics::RGB);
        /**
         * Copy constructor. Performs a deep copy of the passed image.
         */
        constexpr image(image const & other);

        /**
         * The width of the image.
         */
        size_t width() const;
        /**
         * The height of the image.
         */
        size_t height() const;
        /**
         * The number of channels in the image.
         */
        size_t channels() const;
        /**
         * The semantics assigned to the individual channels.
         */
        channel_semantics channel_semantics() const;

        /**
         * Change the dimensions of the image.
         *
         * \param width The new width of the image
         * \param height The new height of the image
         * \param keep_spatial_semantics A bool deciding whether or not pixels
         * should stay in their place (meaning the operation works like
         * cropping) or if values should keep their place in the data vector
         * (which is much faster but may lead to undesirable results for most
         * cases).
         *
         * \remark If `keep_spatial_semantics = false`, the operation requires
         * at most \f$ O(|W * H * C - W' * H' * C'|) \f$ time, where
         * \f$W\f$, \f$H\f$, \f$C\f$ respectively refer to width, height and
         * number of channels of the image.
         *
         * \todo Define runtime limits for `keep_spatial_semantics = true`.
         */
        void resize(size_t width,
            size_t height,
            spice::channel_semantics channels,
            bool keep_spatial_semantics = true);

        /**
         * Accessor to the underlying vector for fast linear data manipulation.
         * \returns a reference to the raw image data
         */
        std::vector<T> & data();
        /**
         * Accessor to the underlying vector for fast linear data manipulation.
         * \returns a const reference to the raw image data
         */
        const std::vector<T> & data() const;

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
        pixel_view<T> operator()(size_t column, size_t row);
        /**
         * Retrieves a reference to the entire pixel located at the indicated
         * coordinates. No bounds checking is performed.
         *
         * \param column
         * \param row
         * \returns a `pixel_view` referring to the pixel at the indicated
         * location
         */
        const pixel_view<T> operator()(size_t column, size_t row) const;
        /**
         * Retrieves a reference to a single channel's value located at the
         * indicated coordinates. No bounds checking is performed.
         *
         * \param column
         * \param row
         * \param channel
         * \returns The value at the indicated location
         */
        T & operator()(size_t column, size_t row, size_t channel);
        /**
         * Retrieves a reference to a single channel's value located at the
         * indicated coordinates. No bounds checking is performed.
         *
         * \param column
         * \param row
         * \param channel
         * \returns The value at the indicated location
         */
        T const & operator()(size_t column, size_t row, size_t channel) const;

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
         * Adds the first image element-wise to the second.
         */
        image<T>& operator+=(image<T> const & rhs);
        /**
         * Subtracts the first image element-wise from the second.
         */
        image<T>& operator-=(image<T> const & rhs);
        /**
         * Multiplies the first image element-wise with the second.
         */
        image<T>& operator*=(image<T> const & rhs);
        /**
         * Divides the first image element-wise from the second.
         */
        image<T>& operator/=(image<T> const & rhs);

        /**
         * Adds the two images element-wise.
         */
        friend image<T> operator+(image<T> lhs, image<T> const & rhs) {
            lhs += rhs;
            return lhs;
        }
        /**
         * Subtracts the two images element-wise.
         */
        friend image<T> operator-(image<T> lhs, image<T> const & rhs) {
            lhs -= rhs;
            return lhs;
        }
        /**
         * Multiplies the two images element-wise.
         */
        friend image<T> operator*(image<T> lhs, image<T> const & rhs) {
            lhs *= rhs;
            return lhs;
        }
        /**
         * Divides the two images element-wise.
         */
        friend image<T> operator/(image<T> lhs, image<T> const & rhs) {
            lhs /= rhs;
            return lhs;
        }
    };
}

#endif // SPICE_IMAGE
