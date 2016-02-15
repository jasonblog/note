# CMake for windows


- CMakeLists.txt 


```sh
set(libfunctionality_DIR "D:/libfunctionality-binary/install/lib/cmake/libfunctionality")
set(OpenCV_DIR "D:/opencv/build")
set(jsoncpp_DIR "D:/jsoncpp-binary/install/lib/cmake/jsoncpp")
set(BOOST_LIBRARYDIR "D:/boost_1_59_0/libs")
set(BOOST_ROOT "D:/boost_1_59_0")
set(BOOST_INCLUDE_DIR "D:/boost_1_59_0")
```


```sh
cmake -G "Visual Studio 12 Win64" ../
```