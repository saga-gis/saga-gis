#!/bin/bash

# Installation of Homebrew
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install.sh)"

# Installation of library dependencies and CMake through Homebrew
brew install wxmac llvm libomp gdal postgis pdal opencv libharu cmake

# Create a 'saga' folder in your home directory, in
# which we want to build SAGA, and move to it
mkdir ~/saga && cd ~/saga

# Use Git to retrieve current SAGA source codes and
# store these to subdirectory 'saga-code'
git clone https://git.code.sf.net/p/saga-gis/code saga-code

# Using Git you can also specify a certain version branch
# % git clone --branch saga-8.4.0 https://git.code.sf.net/p/saga-gis/code saga-code
# or switch to it any time
# % git checkout saga-8.4.0

# Create and move to a further subdirectory, which we
# will use for the compilation itself
mkdir _build && cd _build

# Configure the SAGA build configuration
cmake ../saga-code/saga-gis
# You can list configuration options with
# % cmake -L ../saga-code/saga-gis
# To build SAGA with debug information the command option is
# % cmake -DCMAKE_BUILD_TYPE=Debug ../saga-code/saga-gis

# Now we are prepared to build and install SAGA
make -j2 && sudo make install

# After installation you should be able to run SAGA
# from command line by execution of the 'saga_gui' command
saga_gui
