
![](https://saga-gis.sourceforge.io/_images/logo_saga.png)![](https://saga-gis.sourceforge.io/_images/head_saga_title.png)
#
# __SAGA on macOS__

![](https://licensebuttons.net/l/by-nc-sa/3.0/88x31.png)
#
__SAGA__ installs and works quite well on various Unix-like operating systems, such as _Linux_ and _FreeBSD_. Therefore it should not be problem to make it run on _Darwin_, the base of __macOS__, too. In this document we show how to install SAGA on macOS with help of Apple's development environment __Xcode__, the package management system __Homebrew__, and the __CMake__ application for configuring the SAGA build. Don’t be afraid that this obviously includes the compilation of SAGA from its source codes, ...following the steps in this document it is really easy!

## ___Xcode___

As prerequisites you need to have XCode and CMake being installed on your system. __Xcode__ is the tool developers use to build apps for the Apple ecosystem. It is most easily installed from the [Mac App Store](https://www.apple.com/app-store/), but you might find more options at [Apple's Developer Homepage](https://developer.apple.com/xcode/).

## ___Homebrew___

Homebrew is a free and open-source software package management system dedicated to simplify the installation of software on Apple's macOS operating system. To find further information on Homebrew have a look at the [Homebrew Homepage](https://brew.sh).

Beginning with the Homebrew installation we will exclusively proceed using a terminal or console window, in which we can type the necessary commands. Alternatively you can also type all single commands down into a shell script, which you execute instead. During installation of Homebrew (if you have not installed it yet), you might need to confirm the process with your administrator’s password. The command for the Homebrew installation simply is:
```
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install.sh)"
```
The installation might take a while (and no problem, it is almost as easy to deinstall it again, just in case you don’t want to have it anymore).

## ___Library Dependencies___

Once Homebrew has been installed, you can continue using the __brew__ command with the installation of libraries that need to be present before the SAGA compilation itself can be started:
```
brew install wxmac llvm libomp gdal postgis pdal opencv libharu
```
This will install the packages _wxmac_, _llvm_, _libomp_, _gdal_, _postgis_, _pdal_, _opencv_, and _libharu_. The _wxMac_ package ([The portable C++ GUI toolkit wxWidgets](https://wxwidgets.org/)) is absolutely necessary for a successful SAGA compilation. The other packages are optional but highly recommended. With _llvm_ and _libomp_ parallelization of SAGA routines will be enabled. The _gdal_ package ([Geospatial Data Abstraction Library](https://gdal.org/))  is a translator library for various raster and vector geospatial data formats and will also install the [proj](https://proj.org/) library, which is used by SAGA for all kind of coordinate transformations. With _postgis_ a [PostgreSQL](https://www.postgresql.org/) based geospatial database will also be added. The [PDAL - Point Data Abstraction Library](https://pdal.io/) supports point cloud formats typically used for laser scan data. [OpenCV](https://opencv.org/) installs the _Open Source Computer Vision Library_ for image analysis and classification. The [Haru free PDF Library](http://libharu.org/) is used for generating PDF files.


## ___The CMake Build Configuration Tool___
__CMake__ is an open-source, cross-platform family of tools designed to build, test and package software. Find out more at the [CMake Homepage](https://cmake.org/). Starting with _SAGA 8.4.0_ the SAGA build configuration completely relies on CMake, which makes it pretty much easy to build SAGA on different platforms. You find pre-compiled CMake binaries for macOS available on the [CMake Download](https://cmake.org/download/) page. But because we have already installed Homebrew we can also install _CMake_ with the _brew install_ command:
```
brew install cmake
```
Of course we could have installed _CMake_ in one step together with the library dependencies as explained in the previous chapter. After installation we can use _CMake_ from the command prompt.


## ___Building SAGA___
We continue with the SAGA compilation itself. First I recommend to create a directory, in which the SAGA compilation will be done, and to move to it (after successful compilation you might want to remove this directory again). Here we create a _'saga'_ directory within our home (_'~'_) directory:
```
mkdir ~/saga
cd ~/saga
```
Go and get the latest SAGA source codes directly from the [SAGA Git repository](https://sourceforge.net/p/saga-gis/code/ci/master/tree/):
```
git clone https://git.code.sf.net/p/saga-gis/code saga-code
```
or adjust the following command line, if you want to build a specific version branch:
```
git clone --branch saga-8.4.1 https://git.code.sf.net/p/saga-gis/code saga-code 
```
After the checkout you will find all source codes in the _'saga-code'_ subdirectory. Now let's create a further subdirectory beside it, in which we will run the building process itself and enter it
```
mkdir _build
cd _build 
```
Now running CMake will create the build configuration and later on all intermediate object and program files within this directory
```
cmake ../saga-code/saga-gis 
```
CMake creates so called _'Makefiles'_ which will be recognized by the _'make'_ command for the source code compilation and the _'make install'_ command for the system-wide installation:
```
make && sudo make install
```
The make command will take a bit longer to finish, but after the sudo make install command you should already be able to run SAGA from the command line, just typing:
```
saga_gui
```
That’s it so far! And now enjoy SAGA on macOS...

![](https://saga-gis.sourceforge.io/_screenshots/macos/macos_saga_twi.jpg)

## ___Shell Script___
This chapter is kind of a condensed version of the previous contents. As mentioned above you can collect all commands within one shell script file and execute this instead of running one command after the other. Running the script only expects that _Xcode_ has been installed beforehand. To create the script copy the following commands to a new text file and change its file extension to _'sh'_, e.g. _'build-saga.sh'_.
```
#!/bin/bash

/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install.sh)"

brew install wxmac llvm libomp gdal postgis cmake

mkdir ~/saga && cd ~/saga

git clone https://git.code.sf.net/p/saga-gis/code saga-code

mkdir _build && cd _build

cmake ../saga-code/saga-gis

make -j4 && sudo make install
```
Before you can run the script you also need to flag it as executable. In a terminal you can do this with the _chmod_ command
```
chmod +x build-saga.sh
```
and
```
sh ./build-saga.sh
```
