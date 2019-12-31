/**
 * \file
 * This header defines functions implementing various compositing methods as
 * well as some utility data structures.
 */
#ifndef SPICE_COMPOSITE
#define SPICE_COMPOSITE

#include "image.hpp"
#include "nd_vector.hpp"

namespace spice {
/**
 * Represents a 2D transformation matrix.
 *
 * \example `auto tx = transform_2d().translate(2, 5).rotate(42).scale(2, 1);`
 */
class transform_2d : public nd_vector<float, 2> {
public:
    /**
     * Creates a neutral `2d_transform`.
     */
    transform_2d() : nd_vector({2, 2}) {
        this->m_data[0] = 1;
        this->m_data[2] = 1;
    }

    /**
     * Adds a translation by the given vector to the `transform_2d`.
     */
    transform_2d & translate(float x, float y) {
        // TODO
        return *this;
    }

    /**
     * Adds a rotation by the given angle to the `transform_2d`.
     */
    transform_2d & rotate(float angle) {
        // TODO
        return *this;
    }

    /**
     * Adds a scale by the given factors to the `transform_2d`.
     */
    transform_2d & scale(float x, float y) {
        // TODO
        return *this;
    }
};
}

#endif // SPICE_COMPOSITE