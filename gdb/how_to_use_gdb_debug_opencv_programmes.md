# How to use GDB debug OpenCV programmes?


## install package

```sh
sudo apt-get install build-essential libgtk2.0-dev libjpeg-dev libtiff4-dev libjasper-dev libopenexr-dev cmake python-dev python-numpy python-tk libtbb-dev libeigen2-dev yasm libfaac-dev libopencore-amrnb-dev libopencore-amrwb-dev libtheora-dev libvorbis-dev libxvidcore-dev libx264-dev libqt4-dev libqt4-opengl-dev sphinx-common texlive-latex-extra libv4l-dev libdc1394-22-dev libavcodec-dev libavformat-dev libswscale-dev
```


## download opencv-2.4.13

```sh
wget https://github.com/Itseez/opencv/archive/2.4.13.zip
unzip OpenCV-2.4.13.zip
```


## build

```sh
cd opencv-2.4.13
mkdir debug ; cd debug
cmake -D CMAKE_INSTALL_PREFIX=/home/shihyu/gdb_test/opencv/opencv -D CMAKE_BUILD_TYPE:STRING=RelWithDebInfo -D CMAKE_BUILD_TYPE=DEBUG ..
make -j16 ; make install
```

## set env

```sh
export PKG_CONFIG_PATH=$PKG_CONFIG_PATH:/home/shihyu/gdb_test/opencv/opencv/lib/pkgconfig/

export LD_LIBRARY_PATH=/home/shihyu/gdb_test/opencv/opencv/lib/
```

## build example

```sh
cd opencv-2.4.13/samples/c
./build_all.sh
ldd ./facedetect

linux-vdso.so.1 =>  (0x00007ffed1fc9000)

// 指向編譯出來的函數庫路徑
libopencv_core.so.2.4 => /home/shihyu/gdb_test/opencv/opencv/lib/libopencv_core.so.2.4 (0x00007f2dde90a000)
libopencv_highgui.so.2.4 => /home/shihyu/gdb_test/opencv/opencv/lib/libopencv_highgui.so.2.4 (0x00007f2dde69f000)
libopencv_imgproc.so.2.4 => /home/shihyu/gdb_test/opencv/opencv/lib/libopencv_imgproc.so.2.4 (0x00007f2dde0f7000)
libopencv_objdetect.so.2.4 => /home/shihyu/gdb_test/opencv/opencv/lib/libopencv_objdetect.so.2.4 (0x00007f2ddddbe000)


libstdc++.so.6 => /usr/lib/x86_64-linux-gnu/libstdc++.so.6 (0x00007f2ddda93000)
libgcc_s.so.1 => /lib/x86_64-linux-gnu/libgcc_s.so.1 (0x00007f2ddd87c000)
libc.so.6 => /lib/x86_64-linux-gnu/libc.so.6 (0x00007f2ddd4b7000)
libz.so.1 => /lib/x86_64-linux-gnu/libz.so.1 (0x00007f2ddd29e000)
libpthread.so.0 => /lib/x86_64-linux-gnu/libpthread.so.0 (0x00007f2ddd07f000)
librt.so.1 => /lib/x86_64-linux-gnu/librt.so.1 (0x00007f2ddce77000)
libm.so.6 => /lib/x86_64-linux-gnu/libm.so.6 (0x00007f2ddcb71000)
libjpeg.so.8 => /usr/lib/x86_64-linux-gnu/libjpeg.so.8 (0x00007f2ddc91b000)
libpng12.so.0 => /lib/x86_64-linux-gnu/libpng12.so.0 (0x00007f2ddc6f5000)
libtiff.so.5 => /usr/lib/x86_64-linux-gnu/libtiff.so.5 (0x00007f2ddc483000)
libjasper.so.1 => /usr/lib/x86_64-linux-gnu/libjasper.so.1 (0x00007f2ddc22b000)
libIlmImf.so.6 => /usr/lib/x86_64-linux-gnu/libIlmImf.so.6 (0x00007f2ddbf7c000)
libHalf.so.6 => /usr/lib/x86_64-linux-gnu/libHalf.so.6 (0x00007f2ddbd39000)
libgtk-x11-2.0.so.0 => /usr/lib/x86_64-linux-gnu/libgtk-x11-2.0.so.0 (0x00007f2ddb6fb000)
libgdk-x11-2.0.so.0 => /usr/lib/x86_64-linux-gnu/libgdk-x11-2.0.so.0 (0x00007f2ddb448000)
libgobject-2.0.so.0 => /usr/lib/x86_64-linux-gnu/libgobject-2.0.so.0 (0x00007f2ddb1f7000)
libglib-2.0.so.0 => /lib/x86_64-linux-gnu/libglib-2.0.so.0 (0x00007f2ddaeee000)
libdc1394.so.22 => /usr/lib/x86_64-linux-gnu/libdc1394.so.22 (0x00007f2ddac7a000)
libv4l1.so.0 => /usr/lib/x86_64-linux-gnu/libv4l1.so.0 (0x00007f2ddaa74000)
libv4l2.so.0 => /usr/lib/x86_64-linux-gnu/libv4l2.so.0 (0x00007f2dda865000)
libavcodec.so.54 => /usr/lib/x86_64-linux-gnu/libavcodec.so.54 (0x00007f2dd9b0f000)
libavformat.so.54 => /usr/lib/x86_64-linux-gnu/libavformat.so.54 (0x00007f2dd97ed000)
libavutil.so.52 => /usr/lib/x86_64-linux-gnu/libavutil.so.52 (0x00007f2dd95c7000)
libswscale.so.2 => /usr/lib/x86_64-linux-gnu/libswscale.so.2 (0x00007f2dd9380000)
/lib64/ld-linux-x86-64.so.2 (0x000055f1899d4000)
liblzma.so.5 => /lib/x86_64-linux-gnu/liblzma.so.5 (0x00007f2dd915d000)
libjbig.so.0 => /usr/lib/x86_64-linux-gnu/libjbig.so.0 (0x00007f2dd8f4f000)
libIex.so.6 => /usr/lib/x86_64-linux-gnu/libIex.so.6 (0x00007f2dd8d31000)
libIlmThread.so.6 => /usr/lib/x86_64-linux-gnu/libIlmThread.so.6 (0x00007f2dd8b2a000)
libgmodule-2.0.so.0 => /usr/lib/x86_64-linux-gnu/libgmodule-2.0.so.0 (0x00007f2dd8926000)
libpangocairo-1.0.so.0 => /usr/lib/x86_64-linux-gnu/libpangocairo-1.0.so.0 (0x00007f2dd8719000)
libX11.so.6 => /usr/lib/x86_64-linux-gnu/libX11.so.6 (0x00007f2dd83e3000)
libXfixes.so.3 => /usr/lib/x86_64-linux-gnu/libXfixes.so.3 (0x00007f2dd81dd000)
libatk-1.0.so.0 => /usr/lib/x86_64-linux-gnu/libatk-1.0.so.0 (0x00007f2dd7fbb000)
libcairo.so.2 => /usr/lib/x86_64-linux-gnu/libcairo.so.2 (0x00007f2dd7caf000)
libgdk_pixbuf-2.0.so.0 => /usr/lib/x86_64-linux-gnu/libgdk_pixbuf-2.0.so.0 (0x00007f2dd7a8e000)
libgio-2.0.so.0 => /usr/lib/x86_64-linux-gnu/libgio-2.0.so.0 (0x00007f2dd771b000)
libpangoft2-1.0.so.0 => /usr/lib/x86_64-linux-gnu/libpangoft2-1.0.so.0 (0x00007f2dd7505000)
libpango-1.0.so.0 => /usr/lib/x86_64-linux-gnu/libpango-1.0.so.0 (0x00007f2dd72b8000)
libfontconfig.so.1 => /usr/lib/x86_64-linux-gnu/libfontconfig.so.1 (0x00007f2dd707c000)
libXrender.so.1 => /usr/lib/x86_64-linux-gnu/libXrender.so.1 (0x00007f2dd6e71000)
libXinerama.so.1 => /usr/lib/x86_64-linux-gnu/libXinerama.so.1 (0x00007f2dd6c6e000)
libXi.so.6 => /usr/lib/x86_64-linux-gnu/libXi.so.6 (0x00007f2dd6a5d000)
libXrandr.so.2 => /usr/lib/x86_64-linux-gnu/libXrandr.so.2 (0x00007f2dd6853000)
libXcursor.so.1 => /usr/lib/x86_64-linux-gnu/libXcursor.so.1 (0x00007f2dd6649000)
libXcomposite.so.1 => /usr/lib/x86_64-linux-gnu/libXcomposite.so.1 (0x00007f2dd6446000)
libXdamage.so.1 => /usr/lib/x86_64-linux-gnu/libXdamage.so.1 (0x00007f2dd6242000)
libXext.so.6 => /usr/lib/x86_64-linux-gnu/libXext.so.6 (0x00007f2dd6030000)
libffi.so.6 => /usr/lib/x86_64-linux-gnu/libffi.so.6 (0x00007f2dd5e27000)
libpcre.so.3 => /lib/x86_64-linux-gnu/libpcre.so.3 (0x00007f2dd5be9000)
libraw1394.so.11 => /usr/lib/x86_64-linux-gnu/libraw1394.so.11 (0x00007f2dd59db000)
libusb-1.0.so.0 => /lib/x86_64-linux-gnu/libusb-1.0.so.0 (0x00007f2dd57c4000)
libdl.so.2 => /lib/x86_64-linux-gnu/libdl.so.2 (0x00007f2dd55bf000)
libv4lconvert.so.0 => /usr/lib/x86_64-linux-gnu/libv4lconvert.so.0 (0x00007f2dd5346000)
libxvidcore.so.4 => /usr/lib/x86_64-linux-gnu/libxvidcore.so.4 (0x00007f2dd5007000)
libx264.so.142 => /usr/lib/x86_64-linux-gnu/libx264.so.142 (0x00007f2dd4c71000)
libvpx.so.1 => /usr/lib/x86_64-linux-gnu/libvpx.so.1 (0x00007f2dd4892000)
libvorbisenc.so.2 => /usr/lib/x86_64-linux-gnu/libvorbisenc.so.2 (0x00007f2dd43c3000)
libvorbis.so.0 => /usr/lib/x86_64-linux-gnu/libvorbis.so.0 (0x00007f2dd4195000)
libvo-amrwbenc.so.0 => /usr/lib/x86_64-linux-gnu/libvo-amrwbenc.so.0 (0x00007f2dd3f7b000)
libvo-aacenc.so.0 => /usr/lib/x86_64-linux-gnu/libvo-aacenc.so.0 (0x00007f2dd3d5e000)
libtheoraenc.so.1 => /usr/lib/x86_64-linux-gnu/libtheoraenc.so.1 (0x00007f2dd3b1d000)
libtheoradec.so.1 => /usr/lib/x86_64-linux-gnu/libtheoradec.so.1 (0x00007f2dd3904000)
libspeex.so.1 => /usr/lib/x86_64-linux-gnu/libspeex.so.1 (0x00007f2dd36eb000)
libschroedinger-1.0.so.0 => /usr/lib/x86_64-linux-gnu/libschroedinger-1.0.so.0 (0x00007f2dd3426000)
libopus.so.0 => /usr/lib/x86_64-linux-gnu/libopus.so.0 (0x00007f2dd31de000)
libopenjpeg.so.2 => /usr/lib/x86_64-linux-gnu/libopenjpeg.so.2 (0x00007f2dd2fbc000)
libopencore-amrwb.so.0 => /usr/lib/x86_64-linux-gnu/libopencore-amrwb.so.0 (0x00007f2dd2da7000)
libopencore-amrnb.so.0 => /usr/lib/x86_64-linux-gnu/libopencore-amrnb.so.0 (0x00007f2dd2b7d000)
libmp3lame.so.0 => /usr/lib/x86_64-linux-gnu/libmp3lame.so.0 (0x00007f2dd28f0000)
libgsm.so.1 => /usr/lib/x86_64-linux-gnu/libgsm.so.1 (0x00007f2dd26e1000)
libva.so.1 => /usr/lib/x86_64-linux-gnu/libva.so.1 (0x00007f2dd24cb000)
librtmp.so.0 => /usr/lib/x86_64-linux-gnu/librtmp.so.0 (0x00007f2dd22b0000)
libgnutls.so.26 => /usr/lib/x86_64-linux-gnu/libgnutls.so.26 (0x00007f2dd1ff2000)
libbz2.so.1.0 => /lib/x86_64-linux-gnu/libbz2.so.1.0 (0x00007f2dd1de2000)
libfreetype.so.6 => /usr/lib/x86_64-linux-gnu/libfreetype.so.6 (0x00007f2dd1b3e000)
libxcb.so.1 => /usr/lib/x86_64-linux-gnu/libxcb.so.1 (0x00007f2dd191f000)
libpixman-1.so.0 => /usr/lib/x86_64-linux-gnu/libpixman-1.so.0 (0x00007f2dd1676000)
libxcb-shm.so.0 => /usr/lib/x86_64-linux-gnu/libxcb-shm.so.0 (0x00007f2dd1473000)
libxcb-render.so.0 => /usr/lib/x86_64-linux-gnu/libxcb-render.so.0 (0x00007f2dd126a000)
libselinux.so.1 => /lib/x86_64-linux-gnu/libselinux.so.1 (0x00007f2dd1046000)
libresolv.so.2 => /lib/x86_64-linux-gnu/libresolv.so.2 (0x00007f2dd0e2b000)
libharfbuzz.so.0 => /usr/lib/x86_64-linux-gnu/libharfbuzz.so.0 (0x00007f2dd0bd6000)
libthai.so.0 => /usr/lib/x86_64-linux-gnu/libthai.so.0 (0x00007f2dd09cc000)
libexpat.so.1 => /lib/x86_64-linux-gnu/libexpat.so.1 (0x00007f2dd07a2000)
libudev.so.1 => /lib/x86_64-linux-gnu/libudev.so.1 (0x00007f2dd0590000)
libogg.so.0 => /usr/lib/x86_64-linux-gnu/libogg.so.0 (0x00007f2dd0387000)
liborc-0.4.so.0 => /usr/lib/x86_64-linux-gnu/liborc-0.4.so.0 (0x00007f2dd0104000)
libgcrypt.so.11 => /lib/x86_64-linux-gnu/libgcrypt.so.11 (0x00007f2dcfe84000)
libtasn1.so.6 => /usr/lib/x86_64-linux-gnu/libtasn1.so.6 (0x00007f2dcfc6f000)
libp11-kit.so.0 => /usr/lib/x86_64-linux-gnu/libp11-kit.so.0 (0x00007f2dcfa2d000)
libXau.so.6 => /usr/lib/x86_64-linux-gnu/libXau.so.6 (0x00007f2dcf829000)
libXdmcp.so.6 => /usr/lib/x86_64-linux-gnu/libXdmcp.so.6 (0x00007f2dcf622000)
libgraphite2.so.3 => /usr/lib/x86_64-linux-gnu/libgraphite2.so.3 (0x00007f2dcf3fc000)
libdatrie.so.1 => /usr/lib/x86_64-linux-gnu/libdatrie.so.1 (0x00007f2dcf1f4000)
libcgmanager.so.0 => /lib/x86_64-linux-gnu/libcgmanager.so.0 (0x00007f2dcefd9000)
libnih.so.1 => /lib/x86_64-linux-gnu/libnih.so.1 (0x00007f2dcedc1000)
libnih-dbus.so.1 => /lib/x86_64-linux-gnu/libnih-dbus.so.1 (0x00007f2dcebb6000)
libdbus-1.so.3 => /lib/x86_64-linux-gnu/libdbus-1.so.3 (0x00007f2dce971000)
libgpg-error.so.0 => /lib/x86_64-linux-gnu/libgpg-error.so.0 (0x00007f2dce76c000)

```

## run
```
./facedetect lena.jpg
```




