#!/usr/bin/env bash

# install "simple" dependencies
apt-get install openimageio

# get doxygen, LaTex and m.css set up for documentation
apt-get install doxygen
apt-get install \
    texlive-base \
    texlive-latex-extra \
    texlive-fonts-extra \
    texlive-fonts-recommended
pip3 install jinja2 Pygments
git submodule --init --recursive --remote

# clone, compile and make Google test
git clone https://github.com/google/googletest.git ./external/gtest
cd ./external/gtest
mkdir build
cd build
cmake -Dgtest_build_samples=OFF -Dgtest_build_tests=OFF ../
make
make install