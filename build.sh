#!/bin/bash

mkdir -p build

cd build
# cmake -DCMAKE_BUILD_TYPE=DEBUG -DIDK_DEBUG=ON -G Ninja ../
cmake -DCMAKE_BUILD_TYPE=RELEASE Ninja ../
ninja

# cp ./CA ../
