#!/bin/bash

# Installation of Homebrew
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install.sh)"

# Installation of SAGA dependencies through Homebrew
brew install wxmac llvm libomp gdal postgis

# Create a 'saga' folder in your home directory, in
# which we want to build SAGA, and move to it
mkdir ~/saga && cd ~/saga

# Use Git to retrieve current SAGA source codes and
# store these to subdirectory 'saga-code'
git clone https://git.code.sf.net/p/saga-gis/code saga-code

# Using Git you can also specify a certain release branch
# % git clone --branch release-8.4.0 https://git.code.sf.net/p/saga-gis/code saga-code

# Create and move to a further subdirectory, which we
# will use for the compilation itself
mkdir _build && cd _build

# Assuming you installed the CMake application to your
# Applications folder, we can run the CMake configuration
# either directly with this command:
# % /Applications/CMake.app/Contents/bin/cmake ../saga-code/saga-gis
# or by adding the program location to the PATH variable:
PATH="/Applications/CMake.app/Contents/bin":"$PATH"
cmake ../saga-code/saga-gis

# Now we are prepared to build and install SAGA
make -j2 && sudo make install

# After installation you should be able to run SAGA
saga_gui
