# spice. An image processing library. {#index}

This is an attempt to provide a set of generic tools for image processing. At the core of the library is a type for storing n-dimensional data which intends to translate the flexibility of [NumPy's](https://docs.scipy.org/doc/numpy/reference/generated/numpy.array.html) and [Julia's](https://docs.julialang.org/en/v1/manual/arrays/) Arrays into a low-overhead structure imitating the design of the existing C++ standard library (and also being compatible with it where applicable).

Building on this type, a number of more image-specific types are provided.

This documentation provides in-depth information on the library interface. Please have a look at the [README](https://github.com/janhett/spice) for an overview of the features.

## Building

### Prerequisites

#### For the library

- [C++17](https://en.cppreference.com/w/cpp/compiler_support)
- [CMake 3.10](https://cmake.org/)
- [OpenImageIO](https://github.com/OpenImageIO/oiio)
- [Guideline Support Library](https://github.com/microsoft/GSL) (included as a git submodule, will be pulled by CMake during configuration)

#### For the tests

- [Google Test](https://github.com/google/googletest)

#### For the documentation

- [Doxygen](http://www.doxygen.nl/)
- [m.css](https://github.com/mosra/m.css) (included as a git submodule, will be pulled by CMake during configuration if building documentation is enabled)

### Build It!

```bash
cd <spice repo directory>
mkdir build
cd build
# tests are enabled by default, use option ENABLE_TESTS to override
cmake .. -DCMAKE_INSTALL_PREFIX=<where spice should be installed> [-DENABLE_TESTS=OFF|ON] [-DENABLE_DOCS=OFF|ON]
cmake --build . --config Debug|Release [--target [install] [doc]]
# if you chose to build the tests, you can test if everything worked
tests/spice-test
```

### Notes

spice has been tested to build with clang and gcc on macOS and may or may not build in other configurations. I plan to extend support to other operating systems and eventually also the MSVC platform, but for now, features are the primary focus. For these early days, the library will also remain dependent on the most recent C++ standard (read: I will jump up to C++20 as soon as it's ready and if you have standards that require you to stick to a specific C++ version, this library probably fails more than just this test).