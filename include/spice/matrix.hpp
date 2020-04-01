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
        matrix<T> c(b_columns, a_rows, T{});

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
 * Represents a matrix.
 */
template<typename T>
class matrix : public nd_vector<T, 2> {
public:
    /**
     * Creates a `matrix` initalised to the identity.
     */
    matrix(
        size_t columns = 0,
        size_t rows = 0
    ) : nd_vector<T, 2>({columns, rows})
    {
        for (size_t j = 0; j < std::min(columns, rows); ++j) {
            (*this)(j, j) = 1;
        }
    }

    /**
     * Creates a `matrix` initalised to the given default value.
     */
    matrix(
        size_t columns,
        size_t rows,
        T const & default_value
    ) : nd_vector<T, 2>({columns, rows})
    {
        if (default_value != T{})
            for (size_t j = 0; j < columns * rows; ++j)
                this->m_data[j] = default_value;
    }

    size_t columns()
    { return this->shape()[0]; }

    size_t rows()
    { return this->shape()[1]; }

    matrix operator*(matrix const & rhs)
    {
        return matmul_internal(
            this->m_data,
            rhs.m_data,
            columns(),
            rows(),
            rhs.columns);
    }
};

/**
 * \brief Invert matrix m.
 * 
 * \throw `std::domain_error` if the matrix is not invertible. Only applicable
 * if template aregument `Throws` is set to true.
 * 
 * \param m the matrix to invert
 * \tparam Throws set this to false to presume invertibility
 * \return matrix<T> 
 */
template<typename T, bool Throws = true>
matrix<T> invert(matrix<T> m)
{
    // implementation adapted from https://www.scratchapixel.com/lessons/
    // mathematics-physics-for-computer-graphics/matrix-inverse
    matrix<T> mat(m.columns(), m.rows());
    for (size_t column = 0; column < m.columns(); ++column)
    {
        // Swap row in case our pivot point is not working
        if (m(column, column) == 0)
        {
            size_t big = column;
            for (size_t row = 0; row < m.rows(); ++row)
                if (fabs(m(row, column)) > fabs(m(big, column)))
                    big = row;

            if constexpr(Throws) {
                // Throw if this is a singular matrix
                if (big == column)
                    throw std::domain_error ("Cannot invert singular matrix");
            }

            // Swap rows
            for (size_t row = 0; row < m.rows(); ++row)
            {
                std::swap(m(column, row), m(big, row));
                std::swap(mat(column, row), mat(big, row));
            }
        }

        // Set each row in the column to 0
        for (size_t row = 0; row < m.rows(); ++row)
        {
            if (row != column)
            {
                T coeff = m(row, column) / m(column, column);
                if (coeff != 0)
                {
                    for (size_t j = 0; j < m.columns(); ++j)
                    {
                        m(row, j) -= coeff * m(column, j);
                        mat(row, j) -= coeff * mat(column, j);
                    }
                    // Set the element to 0 for safety
                    m(row, column) = 0;
                }
            }
        }
    }
    // Set each element of the diagonal to 1
    for (size_t row = 0; row < m.rows(); ++row)
    {
        for (size_t column = 0; column < m.columns(); ++column)
        {
            mat(row, column) /= m(row, row);
        }
    }

    return mat;
}
}

#endif // SPICE_MATRIX