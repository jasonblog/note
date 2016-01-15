# Building OSVR


## Compiler: Clang or GCC or G++
- need g++ for 4.8 or newer

- You will need to build jsoncpp and libfunctionality separately from OSVR-Core : see http://wiki.osvr.com/doku.php?id=startingcore

## Required tools/libraries:

- Boost libraries (1.44 or newer) 
    - There is a bug in Boost 1.58.0 which makes it incompatible.
- OpenCV 
- Git
- CMake (3.0 or newer)

```sh
sudo apt-get install libboost1.55-dev markdown libsdl2-dev libboost-thread1.55-dev libboost-filesystem1.55-dev libboost-program-options1.55-dev libusb-1.0-0-dev libopencv-dev 
```

## Build these libraries first

```
mkdir ~/osvr 
cd osvr
```

- libfunctionality library

```sh
git clone https://github.com/osvr/libfunctionality.git
cd libfunctionality
mkdir build ; cd build
cmake .. -DCMAKE_BUILD_TYPE=RelWithDebInfo -DCMAKE_INSTALL_PREFIX=/usr/local/libfunctionality
make -j8
sudo make install
```

- jsoncpp library

```sh
cd ~/osvr
git clone --recursive https://github.com/VRPN/jsoncpp
cd jsoncpp
mkdir build ; cd build
cmake .. -DJSONCPP_WITH_CMAKE_PACKAGE=ON -DJSONCPP_LIB_BUILD_SHARED=OFF -DCMAKE_CXX_FLAGS=-fPIC -DCMAKE_INSTALL_PREFIX=/usr/local/jsoncpp
make -j8
sudo make install
```
- Adding below to .bashrc file

```sh
PKG_CONFIG_PATH=$PKG_CONFIG_PATH:/usr/local/jsoncpp/lib/pkgconfig
export PKG_CONFIG_PATH
```

##Ready to build OSVR-Core

```sh
git clone --recursive https://github.com/OSVR/OSVR-Core.git
cd OSVR-Core
mkdir build ; cd build
cmake .. -DCMAKE_BUILD_TYPE=RelWithDebInfo -DCMAKE_INSTALL_PREFIX=/usr/local/osvr
make -j8
sudo make install
```
- this will run the server, with the default config file, right from within the build tree.

```sh
bin/osvr_server share/osvrcore/osvr_server_config.json
```
## Reference
- https://github.com/OSVR/OSVR-Core/wiki/Linux-Build-Instructions
- https://gist.github.com/rpavlik/13867c53ca097a7b3ebe
