/**
 * \file
 * This header defines functions implementing various compositing methods as
 * well as some utility data structures.
 */
#ifndef SPICE_MATRIX
#define SPICE_MATRIX

#include "image.hpp"
#include "nd_vector.hpp"

namespace spice {

template<typename T>
class matrix;

namespace {
    /**
     * multiplies a and b (assuming column major data order)
     *
     *                        b_columns (6)
     *                        -----------
     *                  |     b b b b b b
     *                  |     b b b b b b
     *     common_dim(4)|     b b b b b b
     *            ------+     b b b b b b
     *
     * a_rows (5)|a a a a     x x x x x x => 5 x 6
     *           |a a a a     x x x x x x
     *           |a a a a     x x x x x x
     *           |a a a a     x x x x x x
     */
    template<class T>
    matrix<T> matmul_internal(
        T const * const a,
        T const * const b,
        size_t common_dim, size_t a_rows, size_t b_columns)
    {
        matrix<T> c(b_columns, a_rows);

        for (size_t x = 0; x < b_columns; ++x) {
            for (size_t y = 0; y < a_rows; ++y) {
                for (size_t idx = 0; idx < common_dim; ++idx) {
                    c.data()[x * a_rows + y] +=
                        a[idx * a_rows + y] *
                        b[x * common_dim + idx];
                }
            }
        }

        return c;
    }
}

/**
 * Represents a 2D transformation matrix.
 *
 * \example `auto tx = transform_2d().translate(2, 5).rotate(42).scale(2, 1);`
 */
template<typename T>
class matrix : public nd_vector_impl<2, T, true> {
public:
    /**
     * Creates a zero-initialised `matrix`.
     */
    matrix(
        size_t columns = 0,
        size_t rows = 0
    ) : nd_vector_impl<2, T, true>({columns, rows})
    {}
};
}

#endif // SPICE_MATRIX