#mkdir -p build
#cd build
cmake . \
    -G "Eclipse CDT4 - Unix Makefiles" \
    -DCMAKE_ECLIPSE_VERSION=4.5 \
    -DCMAKE_ECLIPSE_GENERATE_SOURCE_PROJECT=TRUE

