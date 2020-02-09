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
namespace interpolation {
    /**
     * \brief Nearest neighbour interpolation between pixels adjacent to
     * `position` by returning the value of the pixel to the top-left of
     * `position`.
     * 
     * \todo Create `const` version of this.
     * 
     * \tparam T 
     * \param source 
     * \param position 
     * \return T 
     */
    template<typename T>
    class nearest_neighbor_floor {
    private:
        image<T> & m_img;
    public:
        nearest_neighbor_floor(image<T> & source) :
        m_img(source)
        {}

        pixel_view<T> operator() (float x, float y)
        {
            return m_img(x, y);
        }
    };

    /**
     * \brief Nearest neighbour interpolation between pixels adjacent to
     * `position` by returning the value of the pixel to the top-left of
     * `position`.
     * 
     * \todo Create `const` version of this.
     * 
     * \tparam T 
     * \param source 
     * \param position 
     * \return T 
     */
    template<typename T>
    class nearest_neighbor_round {
    private:
        image<T> & m_img;
    public:
        nearest_neighbor_round(image<T> & source) :
        m_img(source)
        {}

        pixel_view<T> operator() (float x, float y)
        {
            return m_img(std::round(x), std::round(y));
        }
    };

    /**
     * \brief Nearest neighbour interpolation between pixels adjacent to
     * `position`.
     * 
     * This is an alias for `nearest_neighbor_floor`.
     * 
     * \todo Create `const` version of this.
     * 
     * \tparam T 
     * \param source 
     * \param position 
     * \return T 
     */
    template<typename T>
    using nearest_neighbor = nearest_neighbor_floor<T>;

    /**
     * \brief Bilinear interpolation between pixels adjacent to `position`.
     * 
     * \todo Create `const` version of this.
     * 
     * \tparam T 
     * \param source 
     * \param position 
     * \return T 
     */
    template<typename T>
    class bilinear {
    private:
        image<T> & m_img;
    public:
        bilinear(image<T> const & source) :
        m_img(source)
        {}
    };

    /**
     * \brief Bicubic interpolation between pixels adjacent to `position`.
     * 
     * \todo Create `const` version of this.
     * 
     * \tparam T 
     * \param source 
     * \param position 
     * \return T 
     */
    template<typename T>
    class bicubic {
    private:
        image<T> & m_img;
    public:
        bicubic(image<T> const & source) :
        m_img(source)
        {}
    };
};

/**
 * \brief Copy values from image `b`, transformed by `tx`, into image `a`.
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
void merge(image<T> & a, image<T> /* const */ & b, transform_2d tx)
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

    // std::cout << "bb_start: " << bb_start.first << ", " << bb_start.second << "\n";
    // std::cout << "bb_end: " << bb_end.first << ", " << bb_end.second << "\n";

    // transform "backwards"
    auto tx_inv = invert(tx);
    for (size_t x = bb_start.first; x < bb_end.first; ++x) {
        for (size_t y = bb_start.second; y < bb_end.second; ++y) {
            float coordinates[] = {
                static_cast<float>(x), static_cast<float>(y), 1};
            auto src_coordinates = matmul_internal<float>(
                tx_inv.data_ptr(), &coordinates[0],
                3, 3, 1);

            if (src_coordinates(0,0) < 0 || src_coordinates(0,1) < 0 ||
                src_coordinates(0,0) >= b.width() ||
                src_coordinates(0,1) >= b.height())
                continue;
            
            Interpolation itp(b);
            for (size_t chan = 0; chan < a.channels(); ++chan)
                a(x, y, chan) = itp(src_coordinates(0,0),
                src_coordinates(0,1))(chan);
        }
    }
}
};

#endif // SPICE_COMPOSITE
