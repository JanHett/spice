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
void merge(image<T> & a, image<T> const & b, transform_2d tx);
};

#endif // SPICE_COMPOSITE
