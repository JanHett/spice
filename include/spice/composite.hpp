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
         * \tparam T 
         * \param source 
         * \param position 
         * \return T 
         */
        template<typename T>
        color<T> nearest_neighbor_floor(image<T> const & source,
            std::pair<float, float> const & position)
        {
            return source(position.first, position.second);
        }

        /**
         * \brief Nearest neighbour interpolation between pixels adjacent to
         * `position` by returning the value of the pixel to the top-left of
         * `position`.
         * 
         * \tparam T 
         * \param source 
         * \param position 
         * \return T 
         */
        template<typename T>
        color<T> nearest_neighbor_round(image<T> const & source,
            std::pair<float, float> const & position)
        {
            return source(
                std::round(position.first),
                std::round(position.second));
        }

        /**
         * \brief Nearest neighbour interpolation between pixels adjacent to
         * `position`.
         * 
         * This is an alias for `nearest_neighbor_floor`.
         * 
         * \tparam T 
         * \param source 
         * \param position 
         * \return T 
         */
        template<typename T>
        color<T> nearest_neighbor(image<T> const & source,
            std::pair<float, float> const & position)
        {
            return nearest_neighbor_floor(source, position);
        }

        /**
         * \brief Bilinear interpolation between pixels adjacent to `position`.
         * 
         * \tparam T 
         * \param source 
         * \param position 
         * \return T 
         */
        template<typename T>
        color<T> bilinear(image<T> const & source,
            std::pair<float, float> const & position);

        /**
         * \brief Bicubic interpolation between pixels adjacent to `position`.
         * 
         * \tparam T 
         * \param source 
         * \param position 
         * \return T 
         */
        template<typename T>
        color<T> bicubic(image<T> const & source,
            std::pair<float, float> const & position);
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
template<typename T, typename Interpolation>
void merge(image<T> & a, image<T> const & b, transform_2d tx,
Interpolation interpolation = interpolation::bilinear);
};

#endif // SPICE_COMPOSITE
