# 5 分鐘 Linux 0.11 快速構建實驗環境

[src_download](./src/linux-0.11-lab.tar.bz2)


Linux 0.11 Lab
==============

The old Linux kernel source version 0.11 and the integrated experiment environment.

## Contents

- [Introduction](#introduction)

- [Build on Linux](#build-on-linux)
    - [Linux Setup](#linux-setup)
    - [Hack Linux 0.11](#hack-linux-0.11-on-linux)

- [Build on Mac OS X](#build-on-mac-os-x)
    - [Mac OS X Setup](#mac-os-x-setup)
    - [Hack Linux 0.11](#hack-linux-0.11-on-mac)

- [Build on Other Systems (Include Mac OS X and Windows)](#build-on-other-systems)
    - [Setup with Boot2Docker and Dockerfile](#other-systems-setup)
    - [Hack Linux 0.11](#hack-linux-0.11-on-other-systems)

- [Changes](#changes)

<span id="introduction"></span>
## Introduction

- Basic information
    - Homepage: <http://tinylab.org/linux-0.11-lab>
    - Repository: <https://github.com/tinyclub/linux-0.11-lab>

- Features
    - compilable with many different versions of Gcc.
    - has been tested under modern Linux, Mac OS X.
    - add bulit-in qemu and bochs support, include running and debugging.
    - integrate different prebuilt rootfs (floopy, ramdisk and harddisk).
    - allow to generate callgraph of every specified function
    - a Dockerfile added to simplify this setup on other systems (e.g. Windows).


<span id="build-on-linux"></span>
## Build on Linux

<span id="linux-setup"></span>
### Linux Setup

* The Linux distributions: debian and ubuntu are recommended
* Some tools: gcc gdb qemu cscope ctags cflow graphviz

        $ apt-get install vim cscope exuberant-ctags build-essential qemu

* Use with docker (everything is installed by default for you)
    * Install docker

                $ sudo apt-get install software-properties-common
                $ sudo apt-get install python-software-properties
                $ sudo add-apt-repository ppa:dotcloud/lxc-docker
                $ sudo apt-get -y update
                $ sudo apt-get install lxc-docker

    * Build and Start the service

                $ docker build -t tinylab/linux-0.11-lab ./
                $ CONTAINER_ID=$(docker run -d -p 6080:6080 dorowu/ubuntu-desktop-lxde-vnc)
                $ docker logs $CONTAINER_ID | sed -n 1p
                User: ubuntu Pass: ubuntu

    * Access it via `http://localhost:6080/vnc.html` Or `http://172.17.0.1:6080/vnc.html`

                $ docker exec 8222763de ifconfig eth0 | grep "inet addr:"
                inet addr:172.17.0.1  Bcast:0.0.0.0  Mask:255.255.0.0

Optional (bochs emulator instead of qemu)

    $ sudo apt-get install bochs vgabios bochsbios bochs-doc bochs-x libltdl7 bochs-sdl bochs-term
    $ sudo apt-get install graphviz cflow

    $ make switch       // switch to another emulator, between qemu and bochs
    Switch to use emulator: bochs

<span id="hack-linux-0.11-on-linux"></span>
### Hack Linux 0.11

    $ make help		// get help
    $ make  		// compile
    $ make start-hd	// boot it on qemu with hard disk image
    $ make debug-hd	// debug it via qemu & gdb, you'd start gdb to connect it.

    $ gdb images/kernel.sym
    (gdb) target remote :1234
    (gdb) b main
    (gdb) c

                        // to debug the bootsect or setup part, please load the symbol files
    $ gdb boot/bootsect.sym
    $ gdb boot/setup.sym


Optional

    $ echo "add-auto-load-safe-path $PWD/.gdbinit" > ~/.gdbinit  // let gdb auto load the commands in .gdbinit

<span id="build-on-mac-os-x"></span>
## Build on Mac OS X

**Note**: A simpler method is use Boot2Docker with our Dockerfile, see [Build on the other systems](#build-on-other-systems)

<span id="mac-os-x-setup"></span>
### Mac OS X Setup

* Install xcode from "App Store"
* Install Mac package manage tool: MacPorts from http://www.macports.org/install.php

    * Check your OS X version from "About This Mac", for example, Lion
    * Go to the "Mac OS X Package (.pkg) Installer" part and download the corresponding version
    * Self update MacPorts

                $ xcode-select -switch /Applications/Xcode.app/Contents/Developer
                $ sudo port -v selfupdate

* Install cross compiler gcc and binutils

        $ sudo port install qemu

* Install qemu

        $ sudo port install i386-elf-binutils i386-elf-gcc

* Install graphviz and cflow

        $ sudo port install GraphViz
        $ sudo port install cflow

* Install gdb. 'Cause port doesn't provide i386-elf-gdb, use the pre-compiled tools/mac/gdb.xz or download its source and compile it.

        $ cd tools/mac/ ; tar Jxf gdb.xz

Optional

    $ sudo port install cscope
    $ sudo port install ctags

    $ wget ftp://ftp.gnu.org/gnu/gdb/gdb-7.4.tar.bz2
    $ tar -xzvf gdb-7.4.tar.bz2
    $ cd gdb-7.4
    $ ./configure --target=i386-elf
    $ make

<span id="hack-linux-0.11-on-mac"></span>
### Hack Linux 0.11

Same as [Hack Linux-0.11 on Linux](#hack-linux-0.11-on-linux)

<span id="build-on-other-systems"></span>
## Build on Other Systems (include Mac OS X, Windows)

If want to use this Lab on the other systems, such as Windows (and even Mac OS
X), with the Dockerfile support, everything is simplified.

Only need to install the boot2docker tool, which is a lightweight Linux
distribution made specifially to run Docker containers, with this tool and our
Dockerfile, we can simply build a Linux 0.11 Lab on every system.

<span id="other-systems-setup"></span>
### Setup with Boot2Docker and Dockerfile

- Install Boot2Docker and boot into it

    please follow [Boot2Docker Installation](https://github.com/boot2docker/boot2docker#installation).

- Build and Start the service

        $ docker build -t tinylab/linux-0.11-lab ./
        $ CONTAINER_ID=$(docker run -d -p 6080:6080 dorowu/ubuntu-desktop-lxde-vnc)
        $ docker logs $CONTAINER_ID | sed -n 1p
        User: ubuntu Pass: ubuntu

- Access it via `http://localhost:6080/vnc.html` Or `http://172.17.0.1:6080/vnc.html`

        $ docker exec 8222763de ifconfig eth0 | grep "inet addr:"
        inet addr:172.17.0.1  Bcast:0.0.0.0  Mask:255.255.0.0

<span id="hack-linux-0.11-on-other-systems"></span>
### Hack Linux 0.11

Same as [Hack Linux-0.11 on Linux](#hack-linux-0.11-on-linux)

<span id="changes"></span>
## Changes

* 2015-03-15, falcon <wuzhangjin@gmail.com>
    * launch a linux-0.11-lab project, see <http://tinylab.org/linux-0.11-lab>
    * integrate prebuilt rootfs images, no need to install extra images
    * integrate both qemu and bochs support and simplify the testing on emulators
    * add callgraph generation support
    * add docker support, allow to build the whole lab with one command
    * rewrite the document: README.md
    * Fix up "Reset-floppy Called"
    * clean up everything
    * repo: <https://github.com/tinyclub/linux-0.11-lab>

* 2012-04-30, yuanxinyu <yuanxinyu.hangzhou@gmail.com>
    * add Mac OS X support
    * add latest Gcc support
    * repo: <https://github.com/yuanxinyu/Linux-0.11>

* 2011-07-31, tigercn <moonlight.yang@gmail.com>
    * port to new system and gcc
    * see: <http://www.oldlinux.org/oldlinux/viewthread.php?tid=13681>

* 2008-10-15, falcon <wuzhangjin@gmail.com>
    * add gcc 4.3.2 with 32/64bit host support
    * rewrite boot/{bootsect.s,setup.s} with AT&T
    * replace tools/build.c with tools/build.sh
    * see: <http://www.oldlinux.org/oldlinux/viewthread.php?tid=11651&extra=page%3D1>

* 2005, jiong.zhao <gohigh@sh163.net>
    * write a book for Linux 0.11
    * release a version for RedHat 9 with rootfs, compilers and blabla
    * site: <http://www.oldlinux.org>

* 1991, Linus
    * write and release the original Linux 0.11.

