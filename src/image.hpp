#ifndef SPICE_IMAGE
#define SPICE_IMAGE

#import <vector>
#import <array>
#import <stdexcept>
#import <type_traits>
#import <limits>

#import <OpenImageIO/imageio.h>

#import "./nd_vector.hpp"

/**
 * Contains the entire public interface of the library.
 */
namespace spice
{

/**
 * Alias for convenience and forwards compatibility (channel_list might pack
 * much more information in the future: channel to index mapping etc...).
 */
using channel_list = std::vector<std::string>;

template<typename T = float>
using color = nd_vector<T, 1>;

template<typename T = float>
using pixel_view = nd_span<T, 1>;

template<typename T = float>
using column_view = nd_vector<T, 2>;

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

template<typename T>
class image: public spice::nd_vector<T, 3>
{
    channel_list m_channel_semantics;
public:
    image():
    spice::nd_vector<T, 3>(),
    m_channel_semantics{}
    {}

    image(
        size_t width,
        size_t height,
        channel_list channel_semantics = {}):
    spice::nd_vector<T, 3>({width, height, channel_semantics.size()}),
    m_channel_semantics(channel_semantics)
    {}

    image(image const & other):
    spice::nd_vector<T, 3>(other),
    m_channel_semantics(other.m_channel_semantics)
    {}

    /**
     * Constructs a fresh image with the supplied data and shape. Will take
     * ownership of the pointer.
     */
    image(T * const data,
        size_t width,
        size_t height,
        channel_list channel_semantics = {}):
    spice::nd_vector<T, 3>(data, {width, height, channel_semantics.size()}),
    m_channel_semantics(channel_semantics)
    {}

    /**
     * The width of the image.
     */
    [[nodiscard]] size_t width() const
    { return this->m_shape[0]; }
    /**
     * The height of the image.
     */
    [[nodiscard]] size_t height() const
    { return this->m_shape[1]; }
    /**
     * The number of channels in the image.
     */
    [[nodiscard]] size_t channels() const
    { return this->m_shape[2]; }

    /**
     * The meaning assigned to the individual channels.
     */
    [[nodiscard]] channel_list const & channel_semantics() const
    { return m_channel_semantics; }

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

template<typename T>
image<T> transpose (const image<T> & v)
{
    image<T> new_i(v.height(), v.width(), v.channel_semantics());

    for (size_t x = 0; x < v.width(); ++x)
        for (size_t y = 0; y < v.height(); ++y)
            new_i(y, x) = v(x, y);

    return new_i;
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
    T * tx_img_data = new T[spec.width * spec.height * spec.nchannels];
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
        &tx_img_data[0],
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
        transpose(data).data().data()); // transposing image before writing
    out->close();

    return true;
}

}

#endif // SPICE_IMAGE
