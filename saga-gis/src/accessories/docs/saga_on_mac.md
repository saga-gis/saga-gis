# SAGA on Mac
![](https://saga-gis.sourceforge.io/_images/head_saga_title.png)

## The Homebrew Approach

SAGA installs and works quite well on various Linux and FreeBSD. So it should not be problem to make it run on the Unix-like Darwin, the base of macOS. In this document we show how to install SAGA with help of the free and open-source software package management system Homebrew, that is dedicated to simplify the installation of software on Apple's macOS operating system. Don’t be afraid that this also includes the compilation of SAGA from its source code, ...following the steps it is really easy. For further information on Homebrew have a look at the [Homebrew homepage](https://brew.sh).

To get started you only need to open a terminal or console window. And during installation of Homebrew (if you have not installed it yet) you might need to confirm the process with your administrator’s password. The first command:

```
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install.sh)"
```

This will install Homebrew and might take a while (and no problem, it is almost as easy to deinstall it again, just in case you don’t want to have it anymore). Once Homebrew has been installed, you can proceed using the brew command to install some tools and libraries that need to be present before the SAGA compilation can start:

```
brew install automake
brew install pkg-config
brew install libomp
brew install gdal
brew install postgis
brew install wxmac
```

Now you can continue with the SAGA compilation itself. First I recommend to create a directory, in which the SAGA compilation will be done, and to move to it:

```
mkdir develop
cd develop
```

Now go and get the latest SAGA source codes directly from the SAGA GIT repository:
```
git clone https://git.code.sf.net/p/saga-gis/code saga-code
```

or adjust the following command line, if you want to install a specific version:
```
git clone --branch release-7.7.0 https://git.code.sf.net/p/saga-gis/code saga-code 
```

Enter the downloaded folder ‘saga-code’ and then 'saga-gis':
```
cd saga-code/saga-gis 
```

Execute the following commands one after the other:
```
autoreconf -fi
./configure
make
sudo make install
```

For convenience, with the ‘&&‘ operator it is also possible to write the commands in one line:
```
autoreconf -fi && ./configure && make && sudo make install 
```
The make command will take a bit longer to finish, but after the sudo make install command you should already be able to run SAGA from the command line:
```
saga_gui
```

That’s it so far!

And now enjoy SAGA...
