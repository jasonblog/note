Cube3DViewer
============
This is a program that show the position and orientation of cube in 3D scene.  

![screenshot](https://raw.github.com/shengyu7697/Cube3DViewer/master/screenshot/screenshot2.png)  

## Features
* Render position and orientation.
* Receive pose from generator.
* Receive pose from file.
* Receive pose from socket.

## How to Use
```
$ ./cube3dviewer
```

How to use socket:  
start `cube3dviewer` first and start `test_client`.  

## ToDo
* Support .obj file

## System Requirement
Windows, Mac OS, Linux  

## Develop Environment
* OpenGL
* GLEW
* GLFW

### Install GLEW
```
sudo apt-get install libglew-dev
```

### Install GLFW
```
git clone https://github.com/glfw/glfw
cd glfw && git checkout tags/3.2.1
mkdir build && cd build && sudo make install
```

## License
Cube3DViewer is published under the MIT license.  
