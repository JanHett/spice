# spice

[![Build Status](https://travis-ci.org/JanHett/spice.svg?branch=master)](https://travis-ci.org/JanHett/spice)

## A light-weight image processing library.

This is a re-write of a previous project of mine. It attempts to correct some of the design mistakes I made. That being said, it is far from being usable at the moment. In fact, for now it "does" (almost) nothing for the moment. My current goal is to set up the data structures I need to implement actual functionality. I am taking my time with this, because one of the lessons I learned from the predecessor of this project is that data structures make a huge difference for the performance, reliability and usability of a library. Once this basis is established, I will be sure to boast about the added features in the appropriate place (meaning either in the README or in the documentation).

Check out the [documentation](https://janhett.github.io/spice/) for a more in-depth look at what this library does. I have also published an [article](https://medium.com/@janhettenkofer/designing-an-efficient-user-friendly-image-data-structure-a24a8aed4a2d?source=friends_link&sk=5c8808dfabcf1841752c1fb3e8916238) about the considerations that went into the `spice::image` data structure.

### Building

spice does not yet have a fully fledged cmake/make setup. Setting this up is part of getting the library to a usable state and will happen before a proper release.

For now, there is a Python build script called `./tools/build`. Please run it with `-h` to find out how to use it, it is changing much too frequently to keep this README up to date.

The main library depends on OpenImageIO and nothing else. For unit testing, Google Test is required and to build the documentation, you need Doxygen and m.css.