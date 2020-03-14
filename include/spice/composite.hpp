/**
 * \file composite.hpp
 * \author Jan Hettenkofer (jan@hettenkofer.net)
 * \brief Image merging operations and various utilities.
 * \version 0.1
 * \date 2020-01-20
 * 
 * @copyright Copyright (c) 2020
 */

#ifndef SPICE_COMPOSITE
#define SPICE_COMPOSITE

#include <utility>

#include "image.hpp"
#include "transform.hpp"

namespace spice {
/**
 * \brief Interpolation operations on images.
 *
 */
namespace interpolation {
    /**
     * \brief Nearest neighbour interpolation between pixels adjacent to
     * `position` by returning the value of the pixel to the top-left of
     * `position`.
     * 
     * \tparam T type of pixel values
     */
    template<typename T>
    class nearest_neighbor {
    private:
        image<T> const & m_img;
        color<T> const m_default_color;
    public:
        nearest_neighbor(image<T> const & source) :
        m_img(source), m_default_color({m_img.channels()}, T{})
        {}

        color<T> operator() (float x, float y)
        {
            if (x < 0 || y < 0 || x >= m_img.width() || y >= m_img.height())
                return m_default_color;
            return m_img(x, y);
        }
    };

    /**
     * \brief Nearest neighbour interpolation between pixels adjacent to
     * `position` by rounding `position`.
     * 
     * \tparam T type of pixel values
     */
    template<typename T>
    class nearest_neighbor_round {
    private:
        image<T> const & m_img;
        color<T> const m_default_color;
    public:
        nearest_neighbor_round(image<T> const & source) :
        m_img(source), m_default_color({m_img.channels()}, T{})
        {}

        color<T> operator() (float x, float y)
        {
            if (x < 0 || y < 0 || x >= m_img.width() || y >= m_img.height())
                return m_default_color;
            return m_img(std::round(x), std::round(y));
        }
    };

    /**
     * \brief Bilinear interpolation between pixels adjacent to `position`.
     *
     * \tparam T type of pixel values
     */
    template<typename T>
    class bilinear {
    private:
        image<T> const & m_img;
        color<T> const m_default_color;
    public:
        bilinear(image<T> const & source) :
        m_img(source), m_default_color({m_img.channels()}, T{})
        {}

        color<T> operator() (float x, float y)
        {
            auto unit_x_0 = std::floorf(x);
            auto unit_y_0 = std::floorf(y);
            auto unit_x_1 = std::floorf(x + 1);
            auto unit_y_1 = std::floorf(y + 1);

            auto unit_x = x - std::floorf(x);
            auto unit_y = y - std::floorf(y);

            // create corners and initialise with default values
            pixel_view<T> img_00(m_default_color);
            pixel_view<T> img_01(m_default_color);
            pixel_view<T> img_10(m_default_color);
            pixel_view<T> img_11(m_default_color);

            if (unit_x_0 >= 0 && unit_y_0 >= 0 &&
                unit_x_0 < m_img.width() && unit_y_0 < m_img.height())
            {
                img_00.reset(m_img(unit_x_0, unit_y_0));
            }

            if (unit_x_0 >= 0 && unit_y_1 >= 0 &&
                unit_x_0 < m_img.width() && unit_y_1 < m_img.height())
            {
                img_01.reset(m_img(unit_x_0, unit_y_1));
            }

            if (unit_x_1 >= 0 && unit_y_0 >= 0 &&
                unit_x_1 < m_img.width() && unit_y_0 < m_img.height())
            {
                img_10.reset(m_img(unit_x_1, unit_y_0));
            }

            if (unit_x_1 >= 0 && unit_y_1 >= 0 &&
                unit_x_1 < m_img.width() && unit_y_1 < m_img.height())
            {
                img_11.reset(m_img(unit_x_1, unit_y_1));
            }

            return
                img_00 * (1 - unit_x) * (1 - unit_y) +
                img_10 * unit_x * (1 - unit_y) +
                img_01 * (1 - unit_x) * unit_y +
                img_11 * unit_x * unit_y;
        }
    };

    /**
     * \brief Bicubic interpolation between pixels adjacent to `position`.
     *
     * \tparam T type of pixel values
     */
    template<typename T>
    class bicubic {
    private:
        image<T> const & m_img;
    public:
        bicubic(image<T> const & source) :
        m_img(source)
        {}
    };
};

/**
 * \brief Copy values from image `b`, transformed by `tx`, into image `a`.
 *
 * \todo Handle alpha channel?
 *
 * \param a the destination image
 * \param b the source image
 * \param tx the transformation to be applied to image `b`
 * \param interpolation a function to determine intermediate pixel values
 *
 * \tparam T
 * \tparam Interpolation The interpolation function type
 */
template<typename T, typename Interpolation = interpolation::bilinear<T>>
void merge(image<T> & a, image<T> const & b, transform_2d tx)
{
    // calculate bounding box of b under tx
    auto top_left = matmul_internal(
        tx.data_ptr(), std::array<float, 3>{
            0, 0, 1
        }.data(), 3, 3, 1);
    auto top_right = matmul_internal(
        tx.data_ptr(), std::array<float, 3>{
            static_cast<float>(b.width()), 0, 1
        }.data(), 3, 3, 1);
    auto bottom_left = matmul_internal(
        tx.data_ptr(), std::array<float, 3>{
            0, static_cast<float>(b.height()), 1
        }.data(), 3, 3, 1);
    auto bottom_right = matmul_internal(
        tx.data_ptr(), std::array<float, 3>{
            static_cast<float>(b.width()), static_cast<float>(b.height()), 1
        }.data(), 3, 3, 1);

    std::pair<size_t, size_t> bb_start{
        std::clamp(
            std::min({
                top_left(0,0), top_right(0,0),
                bottom_left(0,0), bottom_right(0,0)
            }) - 1,
            0.f,
            static_cast<float>(a.width())
        ), std::clamp(
            std::min({
                top_left(0,1), top_right(0,1),
                bottom_left(0,1), bottom_right(0,1)
            }) - 1,
            0.f,
            static_cast<float>(a.height())
        )
    };

    std::pair<size_t, size_t> bb_end{
        std::clamp(
            std::max({
                top_left(0,0), top_right(0,0),
                bottom_left(0,0), bottom_right(0,0)
            }) + 1,
            0.f,
            static_cast<float>(a.width())
        ), std::clamp(
            std::max({
                top_left(0,1), top_right(0,1),
                bottom_left(0,1), bottom_right(0,1)
            }) + 1,
            0.f,
            static_cast<float>(a.height())
        )
    };

    // ready the interpolation function
    Interpolation itp(b);
    // transform "backwards"
    auto tx_inv = invert(tx);
    for (size_t x = bb_start.first; x < bb_end.first; ++x) {
        for (size_t y = bb_start.second; y < bb_end.second; ++y) {
            float coordinates[] = {
                static_cast<float>(x), static_cast<float>(y), 1};
            auto src_coordinates = matmul_internal<float>(
                tx_inv.data_ptr(), &coordinates[0],
                3, 3, 1);

            a(x, y) = itp(src_coordinates(0,0), src_coordinates(0,1));
        }
    }
}
};

#endif // SPICE_COMPOSITE
