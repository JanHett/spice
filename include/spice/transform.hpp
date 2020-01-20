/**
 * \file composite.hpp
 * \author Jan Hettenkofer (jan@hettenkofer.net)
 * \brief Functions for compositing images as well as utility data structures.
 * \version 0.1
 * \date 2020-01-14
 * 
 * @copyright Copyright (c) 2020
 */

#ifndef SPICE_COMPOSITE
#define SPICE_COMPOSITE

#include <cmath>

#include "image.hpp"
#include "matrix.hpp"
#include "matrix.hpp"

namespace spice {
/**
 * Represents a 2D transformation matrix.
 *
 * \example `auto tx = transform_2d().translate(2, 5).rotate(42).scale(2, 1);`
 */
class transform_2d : public matrix<float> {
public:
    /**
     * Creates a neutral `2d_transform`.
     */
    transform_2d() : matrix<float>(3, 3)
    {
        this->m_data[0] = 1;
        this->m_data[4] = 1;
        this->m_data[8] = 1;
    }

    /**
     * Creates a `2d_transform` describing the indicated transformation.
     * The order of operations is the same as the order of parameters.
     */
    transform_2d(
        float translate_x,
        float translate_y,
        float angle,
        float scale_x,
        float scale_y
    ) : matrix<float>(3, 3)
    {
        // base state
        this->m_data[0] = 1;
        this->m_data[4] = 1;
        this->m_data[8] = 1;

        this->translate(translate_x, translate_y)
        .rotate(angle)
        .scale(scale_x, scale_y);
    }

    /**
     * Moves the data from `other` to `this`, resizing the object as necessary.
     */
    // transform_2d & operator=(std::vector<float> && other)
    // {
    //     delete this->m_data;
    //     this->m_data = std::exchange(other.data(), nullptr);
    //     return *this;
    // }

    /**
     * Adds a translation by the given vector to the `transform_2d`.
     */
    transform_2d & translate(float x, float y)
    {
        matrix<float> new_data = matmul_internal<float>(this->m_data,
            std::array<float, 9>{
                1, 0, 0,
                0, 1, 0,
                x, y, 1
            }.data(), 3, 3, 3);

        std::swap(new_data.data_ptr(), this->m_data);
        return *this;
    }

    /**
     * \brief Get the currently set translation.
     * 
     * \return x and y components of translation vector
     */
    std::pair<float, float> translate()
    {
        return {this->m_data[6], this->m_data[7]};
    }

    /**
     * Adds a rotation by the given angle to the `transform_2d`.
     */
    transform_2d & rotate(float angle, bool radians = false)
    {
        if (!radians)
            angle = angle / (180 / M_PI);

        matrix<float> new_data = matmul_internal<float>(this->m_data,
            std::array<float, 9>{
                std::cos(angle), -1 * std::sin(angle), 0,
                std::sin(angle), std::cos(angle), 0,
                0, 0, 1
            }.data(),
            3, 3, 3);

        std::swap(new_data.data_ptr(), this->m_data);

        return *this;
    }

    /**
     * \brief Get the currently set rotation.
     * 
     * \return rotation angle
     */
    float rotate()
    {
        return std::atan2(this->m_data[1], this->m_data[0]) * (180 / M_PI) * -1;
    }

    /**
     * Adds a scale by the given factors to the `transform_2d`.
     */
    transform_2d & scale(float x, float y)
    {
        matrix<float> new_data = matmul_internal<float>(this->m_data,
            std::array<float, 9>{
                x, 0, 0,
                0, y, 0,
                0, 0, 1
            }.data(), 3, 3, 3);

        std::swap(new_data.data_ptr(), this->m_data);

        return *this;
    }

    /**
     * \brief Get the currently set scale.
     * 
     * \return x and y scale factors
     */
    std::pair<float, float> scale()
    {
        return {
            std::sqrt(
                this->m_data[0] * this->m_data[0] +
                this->m_data[1] * this->m_data[1]),
            std::sqrt(
                this->m_data[3] * this->m_data[3] +
                this->m_data[4] * this->m_data[4])};
    }
};

/**
 * Creates a `transform_2d` describing the translation by the given vector.
 */
transform_2d translate(float x, float y)
{
    return transform_2d(x, y, 0, 0, 0);
}

/**
 * Creates a `transform_2d` describing the rotation by the given angle.
 */
transform_2d rotate(float angle)
{
    return transform_2d(0, 0, angle, 0, 0);
}

/**
 * Creates a `transform_2d` describing the scale by the given factors.
 */
transform_2d scale(float x, float y)
{
    return transform_2d(0, 0, 0, x, y);
}

/**
 * \brief Copy values from image `b`, transformed by `tx`, into image `a`.
 *
 * \param a the destination image
 * \param b the source image
 * \param tx the transformation to be applied to image `b`
 */
template<typename T>
void merge(image<T> & a, image<T> const & b, transform_2d tx);
}

#endif // SPICE_COMPOSITE