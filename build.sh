#!/bin/bash

mkdir -p build
cd build
cmake ../src -G Ninja
ninja -j2
