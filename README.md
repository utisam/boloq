# boloq

A C++ project template of [CMake](http://www.cmake.org/)
to use boost unittest and git.

## How to use

    cd build
    cmake ..
    make -j4 && sudo make install
    sudo ldconfig

## Develop

    cmake -DCMAKE_BUILD_TYPE=Debug ..
    make -j4
    make test

## Documentation

    cd src
    doxygen
