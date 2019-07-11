#!/usr/bin/env bash

# install "simple" dependencies
brew install openimageio

# get doxygen, LaTex and m.css set up for documentation
brew install doxygen
# mactex is massive - we might need to prevent a timeout
travis_wait brew cask install mactex-no-gui
pip3 install jinja2 Pygments
# git submodule --init --recursive --remote

# clone, compile and make Google test
git clone https://github.com/google/googletest.git ./external/gtest
cd ./external/gtest
mkdir build
cd build
cmake -Dgtest_build_samples=OFF -Dgtest_build_tests=OFF ../
make
make install