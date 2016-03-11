rpi-kmod-samples
================

Set of example kernel-modules for the Raspberry Pi (includes basic module sample as well as samples for GPIOs, GPIO-interrupts)


Prerequisites
-------------

To compile the module examples, a cross-compiler for the PI is need. For more details on how to
install a suitable cross-compiler for the Pi, see this [description] (http://elinux.org/RPi_Kernel_Compilation).

Also make sure, to adjust the path to your cross-compiler in `setenv.sh`.


Compilation
-----------

In the top-level directory first make sure your cross-compile environment is set propperly in `setenv.sh`.
Then source it by issuing:

	source ./setenv.sh

Now, build everything with

	make

This may take some time, since it downloads the Pi kernel sources, and also compiles them (otherwise you will 
not be able to produce any modules). 

The resulting modules (the examples and the default modules) could then be found under `sysroot`. For easier 
transfer to the Pi, also an archive `rpi-modtest-sysroot.tar.bz2` is provided. For transfering the modules
to the Pi, again see this [page] (http://elinux.org/RPi_Kernel_Compilation).


Usage
-----

TODO
