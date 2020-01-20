#ifndef SPICE_ND_VECTOR
#define SPICE_ND_VECTOR

#include <array>
#include <vector>
#include <type_traits>
#include <string>
#include <numeric>
#include <gsl/gsl>

namespace spice
{
/**
 * Represents an N-dimensional, discrete field of values of a type.
 * Depending on how the contained data is interpreted, this class can represent
 * a coordinate, a colour, a transformation matrix, an image or an abstract,
 * higher-dimensional collection of elements.
 */
template<size_t Dimensions, typename T, bool Owner = true>
class nd_vector_impl
{
protected:
    T * m_data;
    std::array<size_t, Dimensions> m_shape;
public:
    /**
     * \returns The number of dimensions as specified by the template
     */
    [[nodiscard]] constexpr size_t dimensions() const
    { return Dimensions; }

    /**
     * The shape of an nd_vector is its size in each of the dimensions.
     * The interpretation of the semantics of said shape is up to the user.
     *
     * \returns The shape array describing this nd_vector
     */
    [[nodiscard]] std::array<size_t, Dimensions> const & shape() const
    { return m_shape; }

    /**
     * Calculates the total number of elements contained in this nd_vector.
     *
     * \note Since the actual size is specified as an array of `size_t` and this
     * function returns the product over all its elements, it is possible that
     * the calculation might overflow.
     *
     * \returns The total number of elements
     */
    [[nodiscard]] size_t size() const
    {
        return std::reduce(
            std::next(std::begin(m_shape)),
            std::end(m_shape),
            *std::begin(m_shape),
            std::multiplies<>());
    }

    /**
     * Allows accessing the underlying data directly.
     */
    [[nodiscard]] constexpr gsl::span<T> data()
    { return gsl::span<T>(m_data, size()); }

    /**
     * Allows accessing the underlying data directly. The data is returned in
     * terms of a one-dimensional, non-owning nd_vector.
     */
    [[nodiscard]] constexpr gsl::span<T const> data() const
    { return gsl::span<T const>(m_data, size()); }

    /**
     * Constructs an nd_vector measuring 0 in every dimension, thus containing
     * no values.
     */
    constexpr nd_vector_impl(): m_data(nullptr), m_shape{} {}

    /**
     * Constructs a fresh nd_vector with the supplied data and shape. Owning
     * nd_vectors will take ownership of the pointer, non-owning nd_vectors will
     * leave resource management to the caller.
     */
    constexpr nd_vector_impl(T * const data,
        std::array<size_t, Dimensions> shape):
    m_data(data), m_shape(shape)
    {}

    /**
     * Constructs an nd_vector from another, reusing the other's buffer. This is
     * overridden for owning nd_vectors to create a new buffer.
     */
    constexpr nd_vector_impl(nd_vector_impl const & other):
    m_data(other.m_data), m_shape(other.m_shape)
    {}

    /**
     * Copies the values from `other` to `this`. Only values included in the
     * intersection of the shapes of `other` and `this` are copied, the shape
     * of `this` is not adjusted.
     */
    nd_vector_impl & operator=(nd_vector_impl const & other)
    {
        if (this != &other) {
            size_t smaller_dim = std::min(m_shape[0], other.m_shape[0]);
            for (size_t entry = 0; entry < smaller_dim; ++entry) {
                (*this)[entry] = other[entry];
            }
        }
        return *this;
    }

    /**
     * Moves the data from `other` to `this`, resizing the object as necessary.
     */
    nd_vector_impl & operator=(nd_vector_impl && other);

    /**
     * Sets all values of `this` equal to `value`. The current shape is
     * retained.
     */
    nd_vector_impl & operator=(T const & value)
    {
        for (size_t offset = 0; offset < size(); ++offset)
            m_data[offset] = value;
        return *this;
    }

    /**
     * Returns an element from the nd_vector.
     *
     * \note This overload is activated for one-dimensional nd_vectors.
     *
     * \note No bounds checking is performed (use `nd_vector::at` for this
     * purpose).
     *
     * \note Accessing element values via `nd_vector::operator()` is slightly
     * faster since no intermediate objects have to be constructed.
     * Iterating over `nd_vector::data` is faster still.
     *
     * Calling `my_image[42][47][2]` on an nd_vector representing an RGB image
     * would return the blue channel value in column 43 (index 42), row 48 of
     * the image.
     * 
     * \param index The index of the element to retrieve
     * \returns A reference to an element in the `nd_vector`
     */
    template<bool Enabled = Dimensions == 1,
        std::enable_if_t<Enabled, int> = 0>
    [[nodiscard]] T & operator[](size_t index) {
        return m_data[index];
    }

    /**
     * Returns an element from the nd_vector.
     *
     * \note This overload is activated for one-dimensional nd_vectors.
     *
     * \note No bounds checking is performed (use `nd_vector::at` for this
     * purpose).
     *
     * \note Accessing element values via `nd_vector::operator()` is slightly
     * faster since no intermediate objects have to be constructed.
     * Iterating over `nd_vector::data` is faster still.
     *
     * Calling `my_image[42][47][2]` on an nd_vector representing an RGB image
     * would return the blue channel value in column 43 (index 42), row 48 of
     * the image.
     * 
     * \param index The index of the element to retrieve
     * \returns A reference to an element in the `nd_vector`
     */
    template<bool Enabled = Dimensions == 1,
        std::enable_if_t<Enabled, int> = 0>
    [[nodiscard]] T const & operator[](size_t index) const
    { return m_data[index]; }

    /**
     * Returns a view into the nd_vector. This view will be a non-owning
     * nd_vector of dimensionality `Dimensions - 1`.
     *
     * \note No bounds checking is performed (use `nd_vector::at` for this
     * purpose).
     *
     * \note Accessing element values via `nd_vector::operator()` is slightly
     * faster since no intermediate objects have to be constructed.
     * Iterating over `nd_vector::data` is faster still.
     *
     * Calling `my_image[42][47]` on an nd_vector representing an RGB image
     * would return a one-dimensional `nd_vector` representing the colour in
     * column 43 (index 42), row 48 of the image.
     * 
     * \param index The index of the column to retrieve a view for
     * \returns A non-owning `nd_vector` of decremented dimensionality referring
     * to the dimensional slice indicated by the index.
     */
    template<bool Enabled = (Dimensions > 1),
        std::enable_if_t<Enabled, int> = 0>
    [[nodiscard]] nd_vector_impl<Dimensions - 1, T, false>
    operator[](size_t index)
    {
        size_t data_offset = index * std::reduce(
            std::begin(m_shape) + 2,
            std::end(m_shape),
            *(std::begin(m_shape) + 1),
            std::multiplies<>());

        std::array<size_t, Dimensions - 1> new_shape;
        std::copy(
            std::begin(m_shape) + 1,
            std::end(m_shape),
            std::begin(new_shape));

        return nd_vector_impl<Dimensions - 1, T, false>(
            &m_data[data_offset],
            new_shape);
    }

    /**
     * Returns a view into the nd_vector. This view will be a non-owning
     * nd_vector of dimensionality `Dimensions - 1`.
     *
     * \note No bounds checking is performed (use `nd_vector::at` for this
     * purpose).
     *
     * \note Accessing element values via `nd_vector::operator()` is slightly
     * faster since no intermediate objects have to be constructed.
     * Iterating over `nd_vector::data` is faster still.
     *
     * Calling `my_image[42][47]` on an nd_vector representing an RGB image
     * would return a one-dimensional `nd_vector` representing the colour in
     * column 43 (index 42), row 48 of the image.
     * 
     * \param index The index of the dimension to retrieve a view for
     * \returns A non-owning `nd_vector` of decremented dimensionality referring
     * to the dimensional slice indicated by the index.
     */
    template<bool Enabled = (Dimensions > 1),
        std::enable_if_t<Enabled, int> = 0>
    [[nodiscard]] nd_vector_impl<Dimensions - 1, T, false> const
    operator[](size_t index) const
    {
        size_t data_offset = index * std::reduce(
            std::begin(m_shape) + 2,
            std::end(m_shape),
            *(std::begin(m_shape) + 1),
            std::multiplies<>());

        std::array<size_t, Dimensions - 1> new_shape;
        std::copy(
            std::begin(m_shape) + 1,
            std::end(m_shape),
            std::begin(new_shape));

        return nd_vector_impl<Dimensions - 1, T, false>(
            &m_data[data_offset],
            new_shape);
    }

    /**
     * Creates a non-owning nd_vector referring to a lower-dimensional slice of
     * this nd_vector.
     *
     * \note No bounds checking is performed (use `nd_vector::at` for this
     * purpose).
     *
     * \param indeces The coordinates to retrieve the slice from
     * \returns A non-owning nd_vector of the same type and dimension _n - m_
     * where _n_ is the dimensionality of this vector and _m_ the count of
     * arguments passed to this function.
     */
    template<typename ...Ts,
        std::enable_if_t<(sizeof...(Ts) < Dimensions), int> = 0>
    [[nodiscard]] nd_vector_impl<Dimensions - sizeof...(Ts), T, false>
    operator()(Ts... indeces)
    {
        // take the rear part of the current shape as the shape of the slice
        std::array<size_t, Dimensions - sizeof...(Ts)> sub_shape;
        std::copy(
            std::begin(m_shape) + sizeof...(Ts),
            std::end(m_shape),
            std::begin(sub_shape));

        // calculate starting index of data slice
        size_t indeces_arr[] = {static_cast<size_t>(indeces)...};
        size_t idx = 0;
        for (size_t idx_offset = 0; idx_offset < sizeof...(Ts); ++idx_offset){
            idx += std::reduce(
                std::begin(m_shape) + idx_offset + 1,
                std::end(m_shape),
                1,
                std::multiplies<size_t>()) * indeces_arr[idx_offset];
        }
        T * sub_data = &m_data[idx];

        return nd_vector_impl<Dimensions - sizeof...(Ts), T, false>(
            sub_data,
            sub_shape);
    }

    /**
     * Creates a non-owning nd_vector referring to a lower-dimensional slice of
     * this nd_vector.
     *
     * \note No bounds checking is performed (use `nd_vector::at` for this
     * purpose).
     *
     * \param indeces The coordinates to retrieve the slice from
     * \returns A non-owning nd_vector of the same type and dimension _n - m_
     * where _n_ is the dimensionality of this vector and _m_ the count of
     * arguments passed to this function.
     */
    template<typename ...Ts,
        std::enable_if_t<(sizeof...(Ts) < Dimensions), int> = 0>
    [[nodiscard]] nd_vector_impl<Dimensions - sizeof...(Ts), T, false> const
    operator()(Ts... indeces) const
    {
        // take the rear part of the current shape as the shape of the slice
        std::array<size_t, Dimensions - sizeof...(Ts)> sub_shape;
        std::copy(
            std::begin(m_shape) + sizeof...(Ts),
            std::end(m_shape),
            std::begin(sub_shape));

        // calculate starting index of data slice
        size_t indeces_arr[] = {static_cast<size_t>(indeces)...};
        size_t idx = 0;
        for (size_t idx_offset = 0; idx_offset < sizeof...(Ts); ++idx_offset){
            idx += std::reduce(
                std::begin(m_shape) + idx_offset + 1,
                std::end(m_shape),
                1,
                std::multiplies<size_t>()) * indeces_arr[idx_offset];
        }
        T * sub_data = &m_data[idx];

        return nd_vector_impl<Dimensions - sizeof...(Ts), T, false>(
            sub_data,
            sub_shape);
    }

    /**
     * Retrieves a reference to an element of this nd_vector.
     *
     * \note No bounds checking is performed (use `nd_vector::at` for this
     * purpose).
     *
     * \param indeces The coordinates to retrieve the element from
     * \returns A reference to an element in the nd_vector
     */
    template<typename ...Ts,
        class = std::common_type_t<Ts...>,
        std::enable_if_t<sizeof...(Ts) == Dimensions, int> = 0>
    [[nodiscard]] T & operator()(Ts... indeces)
    {
        size_t indeces_arr[] = {static_cast<size_t>(indeces)...};
        size_t idx = 0;
        for (size_t idx_offset = 0; idx_offset < sizeof...(Ts); ++idx_offset){
            idx += std::reduce(
                std::begin(m_shape) + idx_offset + 1,
                std::end(m_shape),
                1,
                std::multiplies<size_t>()) * indeces_arr[idx_offset];
        }
        return m_data[idx];
    }

    /**
     * Retrieves a reference to an element of this nd_vector.
     *
     * \note No bounds checking is performed (use `nd_vector::at` for this
     * purpose).
     *
     * \param indeces The coordinates to retrieve the element from
     * \returns A const reference to an element in the nd_vector
     */
    template<typename ...Ts,
        class = std::common_type_t<Ts...>,
        std::enable_if_t<sizeof...(Ts) == Dimensions, int> = 0>
    [[nodiscard]] T const & operator()(Ts... indeces) const
    {
        size_t indeces_arr[] = {static_cast<size_t>(indeces)...};
        size_t idx = 0;
        for (size_t idx_offset = 0; idx_offset < sizeof...(Ts); ++idx_offset){
            idx += std::reduce(
                std::begin(m_shape) + idx_offset + 1,
                std::end(m_shape),
                1,
                std::multiplies<size_t>()) * indeces_arr[idx_offset];
        }
        return m_data[idx];
    }

    /**
     * Creates a non-owning nd_vector referring to a lower-dimensional slice of
     * this nd_vector.
     *
     * \exception std::out_of_range if the supplied coordinates are not located
     * inside the bounds of this `nd_vector` as supplied by
     * `nd_vector::shape()`.
     *
     * \param indeces The coordinates to retrieve the slice from
     * \returns A non-owning nd_vector of the same type and dimension _n - m_
     * where _n_ is the dimensionality of this vector and _m_ the count of
     * arguments passed to this function.
     */
    template<typename ...Ts,
        class = std::common_type_t<Ts...>,
        std::enable_if_t<(sizeof...(Ts) < Dimensions), int> = 0>
    [[nodiscard]] nd_vector_impl<Dimensions - sizeof...(Ts), T, false>
    at(Ts... indeces)
    {
        size_t coordinates[] = {static_cast<size_t>(indeces)...};
        for (size_t i = 0; i < sizeof...(Ts); ++i)
            if (coordinates[i] > m_shape[i]) {
                std::string coord_str("{ ");
                for (auto const & coord : coordinates)
                    coord_str += std::to_string(coord) + " ";
                coord_str += "}";
                throw std::out_of_range(
                    "Access at " + coord_str +
                    " exceeds the bounds of this `nd_vector` in dimension " +
                    std::to_string(i) + ".");
            }
        return operator()(indeces...);
    }

    /**
     * Creates a non-owning nd_vector referring to a lower-dimensional slice of
     * this nd_vector.
     *
     * \exception std::out_of_range if the supplied coordinates are not located
     * inside the bounds of this `nd_vector` as supplied by
     * `nd_vector::shape()`.
     *
     * \param indeces The coordinates to retrieve the slice from
     * \returns A non-owning nd_vector of the same type and dimension _n - m_
     * where _n_ is the dimensionality of this vector and _m_ the count of
     * arguments passed to this function.
     */
    template<typename ...Ts,
        class = std::common_type_t<Ts...>,
        std::enable_if_t<(sizeof...(Ts) < Dimensions), int> = 0>
    [[nodiscard]] nd_vector_impl<Dimensions - sizeof...(Ts), T, false> const
    at(Ts... indeces) const
    {
        size_t coordinates[] = {static_cast<size_t>(indeces)...};
        for (size_t i = 0; i < sizeof...(Ts); ++i)
            if (coordinates[i] > m_shape[i]) {
                std::string coord_str("{ ");
                for (auto const & coord : coordinates)
                    coord_str += std::to_string(coord) + " ";
                coord_str += "}";
                throw std::out_of_range(
                    "Access at " + coord_str +
                    " exceeds the bounds of this `nd_vector` in dimension " +
                    std::to_string(i) + ".");
            }
        return operator()(indeces...);
    }

    /**
     * Retrieves a reference to an element of this nd_vector.
     *
     * \exception std::out_of_range if the supplied coordinates are not located
     * inside the bounds of this `nd_vector` as supplied by
     * `nd_vector::shape()`.
     *
     * \param indeces The coordinates to retrieve the element from
     * \returns A reference to an element in the nd_vector
     */
    template<typename ...Ts,
        class = std::common_type_t<Ts...>,
        std::enable_if_t<sizeof...(Ts) == Dimensions, int> = 0>
    [[nodiscard]] T & at(Ts... indeces)
    {
        size_t coordinates[] = {static_cast<size_t>(indeces)...};
        for (size_t i = 0; i < sizeof...(Ts); ++i)
            if (coordinates[i] > m_shape[i]) {
                std::string coord_str("{ ");
                for (auto const & coord : coordinates)
                    coord_str += std::to_string(coord) + " ";
                coord_str += "}";
                throw std::out_of_range(
                    "Access at " + coord_str +
                    " exceeds the bounds of this `nd_vector` in dimension " +
                    std::to_string(i) + ".");
            }
        return operator()(indeces...);
    }

    /**
     * Retrieves a reference to an element of this nd_vector.
     *
     * \exception std::out_of_range if the supplied coordinates are not located
     * inside the bounds of this `nd_vector` as supplied by
     * `nd_vector::shape()`.
     *
     * \param indeces The coordinates to retrieve the element from
     * \returns A reference to an element in the nd_vector
     */
    template<typename ...Ts,
        class = std::common_type_t<Ts...>,
        std::enable_if_t<sizeof...(Ts) == Dimensions, int> = 0>
    [[nodiscard]] T const & at(Ts... indeces) const
    {
        size_t coordinates[] = {static_cast<size_t>(indeces)...};
        for (size_t i = 0; i < sizeof...(Ts); ++i)
            if (coordinates[i] > m_shape[i]) {
                std::string coord_str("{ ");
                for (auto const & coord : coordinates)
                    coord_str += std::to_string(coord) + " ";
                coord_str += "}";
                throw std::out_of_range(
                    "Access at " + coord_str +
                    " exceeds the bounds of this `nd_vector` in dimension " +
                    std::to_string(i) + ".");
            }
        return operator()(indeces...);
    }

    /**
     * Compares two nd_vectors with one another. Two nd_vectors are considered
     * to be equal if they have the same shape and contain the same data.
     */
    [[nodiscard]] friend bool operator==(nd_vector_impl const & lhs,
        nd_vector_impl const & rhs)
    {
        if (lhs.m_shape != rhs.m_shape) return false;
        for (size_t i = 0; i < lhs.size(); ++i)
            if (lhs.m_data[i] != rhs.m_data[i])
                return false;
        return true;
    }

    /**
     * Compares two nd_vectors with one another. `operator!=` is implemented as
     * the negation of `nd_vector::operator==`.
     */
    [[nodiscard]] friend bool operator!=(nd_vector_impl const & lhs,
        nd_vector_impl const & rhs)
    { return !(lhs == rhs); }

    /**
     * Compares an nd_vector with a pointer type. They are considered
     * to be equal if the pointer points to the same address as the nd_vector.
     */
    // friend bool operator==(nd_vector_impl const & lhs, T const * const rhs)
    // { return lhs.m_data == rhs; }
    [[nodiscard]] friend bool operator==(nd_vector_impl const & lhs,
        T const * const rhs)
    {
        size_t sz = lhs.size();
        for (size_t i = 0; i < sz; ++i)
            if (lhs.m_data[i] != rhs[i])
                return false;
        return true;
    }

    /**
     * Compares an nd_vector with a pointer type. `operator!=` is implemented as
     * the negation of `nd_vector::operator==`.
     */
    [[nodiscard]] friend bool operator!=(nd_vector_impl const & lhs,
        T const * const rhs)
    { return !(lhs == rhs); }

    /**
     * Compares an nd_vector with a pointer type. They are considered
     * to be equal if the pointer points to the same address as the nd_vector.
     */
    [[nodiscard]] friend bool operator==(T const * const lhs,
        nd_vector_impl const & rhs)
    { return rhs == lhs; }

    /**
     * Compares an nd_vector with a pointer type. `operator!=` is implemented as
     * the negation of `nd_vector::operator==`.
     */
    [[nodiscard]] friend bool operator!=(T const * const lhs,
        nd_vector_impl const & rhs)
    { return !(lhs == rhs); }

    /**
     * Adds the first nd_vector element-wise to the second.
     * If the two nd_vectors have different dimensions, only the overlapping
     * parts will be taken into account. The nd_vectors will be aligned by their
     * top-left corners.
     */
    template<bool Owner_other>
    nd_vector_impl<Dimensions, T, Owner>& operator+=(
            nd_vector_impl<Dimensions, T, Owner_other> const & rhs)
    {
        size_t dim_magn = std::min(
            m_shape[0],
            rhs.m_shape[0]);
        for (size_t i = 0; i < dim_magn; ++i)
            (*this)[i] += rhs[i];

        return *this;
    }

    /**
     * Adds the scalar `rhs` argument to each element of the nd_vector.
     */
    template<typename T_scalar,
        typename = std::enable_if_t<std::is_arithmetic<T_scalar>::value>>
    nd_vector_impl<Dimensions, T, Owner>& operator+=(
            T_scalar const & rhs)
    {
        size_t sz = size();
        for (size_t i = 0; i < sz; ++i)
            this->m_data[i] += rhs;

        return *this;
    }

    /**
     * Subtracts the first nd_vector element-wise from the second.
     * If the two nd_vectors have different dimensions, only the overlapping
     * parts will be taken into account. The nd_vectors will be aligned by their
     * top-left corners.
     */
    template<bool Owner_other>
    nd_vector_impl<Dimensions, T, Owner>& operator-=(
            nd_vector_impl<Dimensions, T, Owner_other> const & rhs)
    {
        size_t dim_magn = std::min(
            m_shape[0],
            rhs.m_shape[0]);
        for (size_t i = 0; i < dim_magn; ++i)
            (*this)[i] -= rhs[i];

        return *this;
    }

    /**
     * Subtracts the scalar `rhs` argument from each element of the nd_vector.
     */
    template<typename T_scalar,
        typename = std::enable_if_t<std::is_arithmetic<T_scalar>::value>>
    nd_vector_impl<Dimensions, T, Owner>& operator-=(
            T_scalar const & rhs)
    {
        size_t sz = size();
        for (size_t i = 0; i < sz; ++i)
            this->m_data[i] -= rhs;

        return *this;
    }

    /**
     * Multiplies the first nd_vector element-wise with the second.
     * If the two nd_vectors have different dimensions, only the overlapping
     * parts will be taken into account. The nd_vectors will be aligned by their
     * top-left corners.
     */
    template<bool Owner_other>
    nd_vector_impl<Dimensions, T, Owner>& operator*=(
            nd_vector_impl<Dimensions, T, Owner_other> const & rhs)
    {
        size_t dim_magn = std::min(
            m_shape[0],
            rhs.m_shape[0]);
        for (size_t i = 0; i < dim_magn; ++i)
            (*this)[i] *= rhs[i];

        return *this;
    }

    /**
     * Multiplies each element of the nd_vector with the scalar `rhs` argument.
     */
    template<typename T_scalar,
        typename = std::enable_if_t<std::is_arithmetic<T_scalar>::value>>
    nd_vector_impl<Dimensions, T, Owner>& operator*=(
            T_scalar const & rhs)
    {
        size_t sz = size();
        for (size_t i = 0; i < sz; ++i)
            this->m_data[i] *= rhs;

        return *this;
    }

    /**
     * Divides the first nd_vector element-wise from the second.
     * If the two nd_vectors have different dimensions, only the overlapping
     * parts will be taken into account. The nd_vectors will be aligned by their
     * top-left corners.
     */
    template<bool Owner_other>
    nd_vector_impl<Dimensions, T, Owner>& operator/=(
            nd_vector_impl<Dimensions, T, Owner_other> const & rhs)
    {
        size_t dim_magn = std::min(
            m_shape[0],
            rhs.m_shape[0]);
        for (size_t i = 0; i < dim_magn; ++i)
            (*this)[i] /= rhs[i];

        return *this;
    }

    /**
     * Divides each element of the nd_vector by the scalar `rhs` argument.
     */
    template<typename T_scalar,
        typename = std::enable_if_t<std::is_arithmetic<T_scalar>::value>>
    nd_vector_impl<Dimensions, T, Owner>& operator/=(
            T_scalar const & rhs)
    {
        size_t sz = size();
        for (size_t i = 0; i < sz; ++i)
            this->m_data[i] /= rhs;

        return *this;
    }

    /**
     * Adds the two nd_vectors element-wise.
     * If the two nd_vectors have different dimensions, only the overlapping
     * parts will be taken into account. The nd_vectors will be aligned by their
     * top-left corners.
     *
     * \note The first operand will be copied as an owning nd_vector, even if
     * it is merely a view.
     */
    template<bool Owner_rhs>
    [[nodiscard]] friend nd_vector_impl<Dimensions, T, true> operator+(
            nd_vector_impl<Dimensions, T, true> lhs,
            nd_vector_impl<Dimensions, T, Owner_rhs> const & rhs)
    {
        lhs += rhs;
        return lhs;
    }

    /**
     * Adds the scalar `rhs` argument to each element of the nd_vector.
     *
     * \note The first operand will be copied as an owning nd_vector, even if
     * it is merely a view.
     */
    template<typename T_scalar,
        typename = std::enable_if_t<std::is_arithmetic<T_scalar>::value>>
    [[nodiscard]] friend nd_vector_impl<Dimensions, T, true> operator+(
            nd_vector_impl<Dimensions, T, true> lhs,
            T_scalar const & rhs)
    {
        lhs += rhs;
        return lhs;
    }

    /**
     * Adds the scalar `lhs` argument to each element of the nd_vector.
     *
     * \note The second operand will be copied as an owning nd_vector, even if
     * it is merely a view.
     */
    template<typename T_scalar,
        typename = std::enable_if_t<std::is_arithmetic<T_scalar>::value>>
    [[nodiscard]] friend nd_vector_impl<Dimensions, T, true> operator+(
            T_scalar const & lhs,
            nd_vector_impl<Dimensions, T, true> rhs)
    {
        rhs += lhs;
        return rhs;
    }

    /**
     * Subtracts the two nd_vectors element-wise.
     * If the two nd_vectors have different dimensions, only the overlapping
     * parts will be taken into account. The nd_vectors will be aligned by their
     * top-left corners.
     *
     * \note The first operand will be copied as an owning nd_vector, even if
     * it is merely a view.
     */
    template<bool Owner_rhs>
    [[nodiscard]] friend nd_vector_impl<Dimensions, T, true> operator-(
            nd_vector_impl<Dimensions, T, true> lhs,
            nd_vector_impl<Dimensions, T, Owner_rhs> const & rhs)
    {
        lhs -= rhs;
        return lhs;
    }

    /**
     * Subtracts the scalar `rhs` argument from each element of the nd_vector.
     *
     * \note The first operand will be copied as an owning nd_vector, even if
     * it is merely a view.
     */
    template<typename T_scalar,
        typename = std::enable_if_t<std::is_arithmetic<T_scalar>::value>>
    [[nodiscard]] friend nd_vector_impl<Dimensions, T, true> operator-(
            nd_vector_impl<Dimensions, T, true> lhs,
            T_scalar const & rhs)
    {
        lhs -= rhs;
        return lhs;
    }

    /**
     * Subtracts the scalar from each element of the nd_vector, returning an
     * nd_vector containing the resulting values.
     *
     * \note The second operand will be copied as an owning nd_vector, even if
     * it is merely a view.
     */
    template<typename T_scalar,
        typename = std::enable_if_t<std::is_arithmetic<T_scalar>::value>>
    [[nodiscard]] friend nd_vector_impl<Dimensions, T, true> operator-(
            T_scalar const & lhs,
            nd_vector_impl<Dimensions, T, true> rhs)
    {
        for (auto & el : rhs.data())
            el = lhs - el;
        return rhs;
    }

    /**
     * Multiplies the two nd_vectors element-wise.
     * If the two nd_vectors have different dimensions, only the overlapping
     * parts will be taken into account. The nd_vectors will be aligned by their
     * top-left corners.
     *
     * \note The first operand will be copied as an owning nd_vector, even if
     * it is merely a view.
     */
    template<bool Owner_rhs>
    [[nodiscard]] friend nd_vector_impl<Dimensions, T, true> operator*(
            nd_vector_impl<Dimensions, T, true> lhs,
            nd_vector_impl<Dimensions, T, Owner_rhs> const & rhs)
    {
        lhs *= rhs;
        return lhs;
    }

    /**
     * Multiplies each element of the nd_vector with the scalar `rhs` argument.
     *
     * \note The first operand will be copied as an owning nd_vector, even if
     * it is merely a view.
     */
    template<typename T_scalar,
        typename = std::enable_if_t<std::is_arithmetic<T_scalar>::value>>
    [[nodiscard]] friend nd_vector_impl<Dimensions, T, true> operator*(
            nd_vector_impl<Dimensions, T, true> lhs,
            T_scalar const & rhs)
    {
        lhs *= rhs;
        return lhs;
    }

    /**
     * Multiplies each element of the nd_vector with the scalar `lhs` argument.
     *
     * \note The second operand will be copied as an owning nd_vector, even if
     * it is merely a view.
     */
    template<typename T_scalar,
        typename = std::enable_if_t<std::is_arithmetic<T_scalar>::value>>
    [[nodiscard]] friend nd_vector_impl<Dimensions, T, true> operator*(
            T_scalar const & lhs,
            nd_vector_impl<Dimensions, T, true> rhs)
    {
        rhs *= lhs;
        return rhs;
    }

    /**
     * Divides the two nd_vectors element-wise.
     * If the two nd_vectors have different dimensions, only the overlapping
     * parts will be taken into account. The nd_vectors will be aligned by their
     * top-left corners.
     *
     * \note The first operand will be copied as an owning nd_vector, even if
     * it is merely a view.
     */
    template<bool Owner_rhs>
    [[nodiscard]] friend nd_vector_impl<Dimensions, T, true> operator/(
            nd_vector_impl<Dimensions, T, true> lhs,
            nd_vector_impl<Dimensions, T, Owner_rhs> const & rhs)
    {
        lhs /= rhs;
        return lhs;
    }

    /**
     * Divides each element of the nd_vector by the scalar `rhs` argument.
     *
     * \note The first operand will be copied as an owning nd_vector, even if
     * it is merely a view.
     */
    template<typename T_scalar,
        typename = std::enable_if_t<std::is_arithmetic<T_scalar>::value>>
    [[nodiscard]] friend nd_vector_impl<Dimensions, T, true> operator/(
            nd_vector_impl<Dimensions, T, true> lhs,
            T_scalar const & rhs)
    {
        lhs /= rhs;
        return lhs;
    }

    /**
     * Divides the scalar by each element of the nd_vector, returning an
     * nd_vector containing the resulting values.
     *
     * \note The second operand will be copied as an owning nd_vector, even if
     * it is merely a view.
     */
    template<typename T_scalar,
        typename = std::enable_if_t<std::is_arithmetic<T_scalar>::value>>
    [[nodiscard]] friend nd_vector_impl<Dimensions, T, true> operator/(
            T_scalar const & lhs,
            nd_vector_impl<Dimensions, T, true> rhs)
    {
        for (auto & el : rhs.data())
            el = lhs / el;
        return rhs;
    }

    /**
     * Prints a representation of the nd_vector of the form
     * `nd_vector_impl (W x H x D)[ 42, ..., 47 ]`to the proVided stream.
     */
    friend std::ostream& operator<<(std::ostream& os,
        nd_vector_impl const & vec)
    {
        os << std::string("nd_vector_impl (");
        for (size_t i = 0; i < Dimensions - 1; ++i)
            os << vec.m_shape[i] << " x ";
        os << vec.m_shape[Dimensions - 1] << ")[";
        for (size_t i = 0; i < vec.size() - 1; ++i)
            os << vec.m_data[i] << ", ";
        os << vec.m_data[vec.size() - 1] << "]";
        return os;
    }
};

/**
 * Specialisation of spice::nd_vector template class for owning vectors.
 */
template<size_t Dimensions, typename T>
class nd_vector_impl<Dimensions, T, true>:
public nd_vector_impl<Dimensions, T, false>
{
public:
    using nd_vector_impl<Dimensions, T, false>::nd_vector_impl;
    using nd_vector_impl<Dimensions, T, false>::operator=;

    /**
     * Constructs a fresh nd_vector with the specified shape, initialising the
     * data with the provided default value (or `T{}` if none is passed).
     */
    explicit nd_vector_impl(std::array<size_t, Dimensions> shape,
        T const & default_value = T{}):
    // initialise superclass with `nd_vector_impl(T*, array<size_t, Dimensions>)`
    // size of data array has to be calculated from shape
    nd_vector_impl<Dimensions, T, false>(new T[std::reduce(
            std::next(std::begin(shape)),
            std::end(shape),
            *std::begin(shape),
            std::multiplies<>())], shape)
    {
        // initialise with 0 (or equivalent value)
        for (size_t i = 0; i < this->size(); ++i)
            this->m_data[i] = default_value;
    }

    /**
     * Copy constructor.
     * Copies the values and shape from `other` to `this`.
     */
    nd_vector_impl(nd_vector_impl<Dimensions, T, true> const & other):
    // initialise superclass with `nd_vector_impl(T*, array<size_t, Dimensions>)`
    // size of data array has to be calculated from other's shape
    nd_vector_impl<Dimensions, T, false>(new T[other.size()], other.shape())
    {
        // copy values over
        for (size_t i = 0; i < this->size(); ++i)
            this->m_data[i] = other.data()[i];
    }

    /**
     * Copies the values and shape from `other` to `this`.
     */
    nd_vector_impl(nd_vector_impl<Dimensions, T, false> const & other):
    // initialise superclass with `nd_vector_impl(T*, array<size_t, Dimensions>)`
    // size of data array has to be calculated from other's shape
    nd_vector_impl<Dimensions, T, false>(new T[other.size()], other.shape())
    {
        // copy values over
        for (size_t i = 0; i < this->size(); ++i)
            this->m_data[i] = other.data()[i];
    }

    /**
     * Moves the data from `other` to `this`.
     */
    nd_vector_impl(nd_vector_impl && other) noexcept:
            nd_vector_impl(
                std::exchange(other.m_data, nullptr),
                std::move(other.shape()))
    {}

    /**
     * Constructs a fresh nd_vector with the supplied data and shape.
     */
    nd_vector_impl(std::initializer_list<T> data,
                   std::array<size_t, Dimensions> shape);

    /**
     * Destructor. Deletes data array only if this nd_vector owns it (i.e. if
     * the template argument `Owner` is `true`).
     */
    ~nd_vector_impl()
    {
        delete[] this->m_data;
    }

    /**
     * \brief Get a reference to the data pointer.
     * 
     * \return T*& 
     */
    [[nodiscard]] T * & data_ptr()
    { return this->m_data; }

    /**
     * Copies the values from `other` to `this`. Unlike the copy assignment
     * operator of `nd_span`, this copies the entire `nd_vector` and also
     * modifies the size of it.
     */
    nd_vector_impl & operator=(nd_vector_impl const & other)
    {
        if (this != &other) {
            delete this->m_data;
            auto buf_size = other.size();
            this->m_data = new T[buf_size];
            std::memcpy(this->m_data, other.m_data, buf_size * sizeof(T));
            this->m_shape = other.m_shape;
        }
        return *this;
    }

    /**
     * Moves the data from `other` to `this`, resizing the object as necessary.
     */
    nd_vector_impl & operator=(nd_vector_impl && other)
    {
        // std::cout << "Moving nd_vector...\n";
        if (this != &other) {
            delete this->m_data;
            this->m_data = std::exchange(other.m_data, nullptr);
            this->m_shape = std::exchange(other.m_shape, {});
        }
        return *this;
    }
};

/**
 * Non-owning ``nd_vector_impl`` providing a view over a slice of an 
 * ``nd_vector``. Can also be used to wrap a raw pointer representing the start
 * of a multidimensional buffer.
 */
template<typename T, size_t Dimensions>
using nd_span = nd_vector_impl<Dimensions, T, false>;

/**
 * Owning ``nd_vector_impl`` managing its own buffer.
 */
template<typename T, size_t Dimensions>
using nd_vector = nd_vector_impl<Dimensions, T, true>;

}

#endif // SPICE_ND_VECTOR
