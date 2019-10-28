#!/usr/bin/env bash

mkdir build
cd build
cmake .. -DCMAKE_INSTALL_PREFIX=../../spice_install
cmake --build . --config Debug
tests/spice-test
cmake --build . --config Debug --target doc
