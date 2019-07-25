#ifndef SPICE_ND_VECTOR
#define SPICE_ND_VECTOR

#include <array>
#include <vector>
#include <type_traits>
#include <string>
#include <numeric>

namespace spice
{
/**
 * Represents an N-dimensional, discrete field of values of a type.
 * Depending on how the contained data is interpreted, this class can represent
 * a coordinate, a colour, a transformation matrix, an image or an abstract,
 * higher-dimensional collection of elements.
 */
template<size_t Dimensions, typename T, bool Owner = true>
class nd_vector
{
protected:
    T * m_data;
    std::array<size_t, Dimensions> m_shape;
public:
    /**
     * \returns The number of dimensions as specified by the template
     */
    constexpr size_t dimensions() const
    { return Dimensions; }

    /**
     * The shape of an nd_vector is its size in each of the dimensions.
     * The interpretation of the semantics of said shape is up to the user.
     *
     * \returns The shape array describing this nd_vector
     */
    std::array<size_t, Dimensions> const & shape() const
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
    size_t size() const
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
    constexpr T * const data()
    { return m_data; }

    /**
     * Allows accessing the underlying data directly. The data is returned in
     * terms of a one-dimensional, non-owning nd_vector.
     */
    constexpr T const * data() const
    { return m_data; }

    /**
     * Constructs an nd_vector measuring 0 in every dimension, thus containing
     * no values.
     */
    constexpr nd_vector(): m_data(nullptr), m_shape{} {}
    /**
     * Constructs a fresh nd_vector with the supplied data and shape. Owning
     * nd_vectors will take ownership of the pointer, non-owning nd_vectors will
     * leave resource management to the caller.
     */
    constexpr nd_vector(T * const data, std::array<size_t, Dimensions> shape):
    m_data(data), m_shape(shape)
    {}

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
    T & operator[](size_t index) {
        return this->m_data[index];
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
    T const & operator[](size_t index) const;

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
    nd_vector<Dimensions - 1, T, false> operator[](size_t index)
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

        return nd_vector<Dimensions - 1, T, false>(
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
    nd_vector<Dimensions - 1, T, false> const operator[](size_t index) const
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

        return nd_vector<Dimensions - 1, T, false>(
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
    nd_vector<Dimensions - sizeof...(Ts), T, false> operator()(Ts... indeces);
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
    nd_vector<Dimensions - sizeof...(Ts), T, false> const
    operator()(Ts... indeces) const;

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
        std::enable_if_t<sizeof...(Ts) == Dimensions, int> = 0>
    T & operator()(Ts... indeces);
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
        std::enable_if_t<sizeof...(Ts) == Dimensions, int> = 0>
    T const & operator()(Ts... indeces) const;

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
        std::enable_if_t<(sizeof...(Ts) < Dimensions), int> = 0>
    nd_vector<Dimensions - sizeof...(Ts), T, false> at(Ts... indeces)
    {
        std::vector<size_t> coordinates = {indeces...};
        for (size_t i = 0; i < coordinates.size(); ++i)
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
        std::enable_if_t<(sizeof...(Ts) < Dimensions), int> = 0>
    nd_vector<Dimensions - sizeof...(Ts), T, false> const
    at(Ts... indeces) const
    {
        std::vector<size_t> coordinates = {indeces...};
        for (size_t i = 0; i < coordinates.size(); ++i)
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
        std::enable_if_t<sizeof...(Ts) == Dimensions, int> = 0>
    T & at(Ts... indeces)
    {
        std::vector<size_t> coordinates = {indeces...};
        for (size_t i = 0; i < coordinates.size(); ++i)
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
        std::enable_if_t<sizeof...(Ts) == Dimensions, int> = 0>
    T const & at(Ts... indeces) const
    {
        std::vector<size_t> coordinates = {indeces...};
        for (size_t i = 0; i < coordinates.size(); ++i)
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
    friend bool operator==(nd_vector const & lhs, nd_vector const & rhs)
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
    friend bool operator!=(nd_vector const & lhs, nd_vector const & rhs)
    { return !(lhs == rhs); }

    /**
     * Compares an nd_vector with a pointer type. They are considered
     * to be equal if the pointer points to the same address as the nd_vector.
     */
    friend bool operator==(nd_vector const & lhs, T const * const rhs)
    { return lhs.m_data == rhs; }

    /**
     * Compares an nd_vector with a pointer type. `operator!=` is implemented as
     * the negation of `nd_vector::operator==`.
     */
    friend bool operator!=(nd_vector const & lhs, T const * const rhs)
    { return !(lhs == rhs); }

    /**
     * Compares an nd_vector with a pointer type. They are considered
     * to be equal if the pointer points to the same address as the nd_vector.
     */
    friend bool operator==(T const * const lhs, nd_vector const & rhs)
    { return lhs == rhs.m_data; }

    /**
     * Compares an nd_vector with a pointer type. `operator!=` is implemented as
     * the negation of `nd_vector::operator==`.
     */
    friend bool operator!=(T const * const lhs, nd_vector const & rhs)
    { return !(lhs == rhs); }
};

/**
 * Specialisation of spice::nd_vector template class for owning vectors.
 */
template<size_t Dimensions, typename T>
class nd_vector<Dimensions, T, true>:
public nd_vector<Dimensions, T, false>
{
public:
    using nd_vector<Dimensions, T, false>::nd_vector;

    /**
     * Constructs a fresh nd_vector with the specified shape, initialising the
     * data with `T{}`.
     */
    nd_vector(std::array<size_t, Dimensions> shape):
    // initialise superclass with `nd_vector(T*, array<size_t, Dimensions>)`
    // size of data array has to be calculated from shape
    nd_vector<Dimensions, T, false>(new T[std::reduce(
            std::next(std::begin(shape)),
            std::end(shape),
            *std::begin(shape),
            std::multiplies<>())], shape)
    {
        // initialise with 0 (or equivalent value)
        for (size_t i = 0; i < this->size(); ++i)
            this->m_data[i] = T{};
    }
    /**
     * Copies the values and shape from `other` to `this`.
     */
    template<bool Owner_other>
    nd_vector(nd_vector<Dimensions, T, Owner_other> const & other):
    // initialise superclass with `nd_vector(T*, array<size_t, Dimensions>)`
    // size of data array has to be calculated from other's shape
    nd_vector<Dimensions, T, false>(new T[other.size()], other.shape())
    {
        // copy values over
        for (size_t i = 0; i < this->size(); ++i)
            this->m_data[i] = other.data()[i];
    }
    /**
     * Moves the data from `other` to `this`.
     */
    nd_vector(nd_vector && other) noexcept:
    nd_vector(std::exchange(other.m_data, nullptr), std::move(other.shape())) {}

    /**
     * Constructs a fresh nd_vector with the supplied data and shape.
     */
    nd_vector(std::initializer_list<T> data,
        std::array<size_t, Dimensions> shape);

    /**
     * Destructor. Deletes data array only if this nd_vector owns it (i.e. if
     * the template argument `Owner` is `true`).
     */
    ~nd_vector()
    {
        delete[] this->m_data;
    }

    /**
     * Copies the values from `other` to `this`, resizing the object as
     * necessary.
     */
    nd_vector & operator=(nd_vector const & other);
    // {
    //     if (this != &other) {
    //         if (other.size() != size()){
    //             delete[] m_data;
    //             m_shape.fill(0);
    //             m_data = nullptr;
    //             m_data = new T[other.size()];
    //             m_shape = other.m_shape;
    //         }
    //     }
    //     return *this;
    // }

    /**
     * Moves the data from `other` to `this`, resizing the object as necessary.
     */
    nd_vector & operator=(nd_vector && other);
};

}

#endif // SPICE_ND_VECTOR
