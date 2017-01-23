# Debugging CPython with gdb


##Step 1—We need an environment for building and compiling code written in C. build-essential cuts to the chase and us what we need. We’ll also grab gdb, if we don’t already have it.

```sh
$ sudo apt-get install build-essential gdb
```

##Step 2—Let’s grab the source code for the latest Python straight from python.org:

Python.org downloads page

```sh
# for Python 3
$ wget 'http://python.org/ftp/python/3.3.1/Python-3.3.1.tar.xz'

# for Python 2 (what I'll use)
$ wget 'http://python.org/ftp/python/2.7.4/Python-2.7.4.tar.xz'
```

##Step 3—We should extract these. .xz is LZMA2 compression, which is available via the -J toggle in tar.

```sh
$ tar xJvf Python-2.7.4.tar.xz
```

## Step 4—We also need a couple of other shared libraries to build Python. It can be a bit onerous to track these down, compile, and install them in turn, so we’ll use the following trick.

```sh
# we'll use apt to pull and install all the build dependencies for our system's python 2.7
$ sudo apt-get build-dep python2.7
```

##Step 5—Let’s configure, make, make install.

```sh
# these CFLAGS are necessary to export extra debugging information for use with gdb
# gdwarf-4 probably requires gdb 7.0 or higher
$ CFLAGS="-g3 -ggdb -gdwarf-4" ./configure --with-pydebug --prefix=$PWD-build
# at the end of this next step, we'll have Python installed to ../Python-2.7.4-build
$ make && make install
```

##Step 6—Let’s try it out.

```sh
$ gdb -- Python-2.7.4-build/bin/python
(gdb) run # let's start the Python interpreter

>>> # we have a running Python interpreter
... # we started the interpreter, so that we could get past all of the
... #   standard startup code that would otherwise trigger our breakpoints
... # let's suspend the process by sending SIGSTP: press control+z
(gdb) # now we're back in gdb
(gdb) tbreak ceval.c:build_class # put a temporary breakpoint in the
(gdb)                            #   interpreter loop on class creation
(gdb) c # let's continue the interactive interpreter
(gdb) c # once more time
>>> class Foo(object): pass
...
(gdb) # let's print name of the class we're building, "Foo"
(gdb) print PyString_AsString(name)
```
