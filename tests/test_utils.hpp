#include <spice/image.hpp>

using namespace spice;

/**
 * \brief Create a checkerboard image. The top-left pixel will be white.
 * 
 * \tparam T 
 * \param width 
 * \param height 
 * \param channel_semantics 
 * \return image<T> 
 */
template<typename T>
image<T> make_checkerboard(size_t width = 1,
    size_t height = 1,
    channel_list channel_semantics = { "R", "G", "B" })
{
    image<T> im(width, height, channel_semantics);
    for (typename gsl::span<T>::index_type pxl = 0; pxl < im.data().size();\
        pxl += im.channels())
        for (size_t chan = 0; chan < im.channels(); ++chan)
            im.data()[pxl + chan] = (pxl / 3) % 2 == 0 ?
                image<T>::intensity_range.max :
                image<T>::intensity_range.min;

    return im;
}
