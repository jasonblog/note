<<<<<<< HEAD
<<<<<<< HEAD
Google C++ Mocking Framework
============================
=======
## Google C++ Testing Framework ##
>>>>>>> readme merging

<http://github.com/google/googlemock/>

### Overview ###

Google's framework for writing and using C++ mock classes on a variety
of platforms (Linux, Mac OS X, Windows, Windows CE, Symbian, etc).
Inspired by jMock, EasyMock, and Hamcrest, and designed with C++'s
specifics in mind, it can help you derive better designs of your
system and write better tests.

Google Mock:

- provides a declarative syntax for defining mocks,
- can easily define partial (hybrid) mocks, which are a cross of real
  and mock objects,
- handles functions of arbitrary types and overloaded functions,
- comes with a rich set of matchers for validating function arguments,
- uses an intuitive syntax for controlling the behavior of a mock,
- does automatic verification of expectations (no record-and-replay
  needed),
- allows arbitrary (partial) ordering constraints on
  function calls to be expressed,
- lets a user extend it by defining new matchers and actions.
- does not use exceptions, and
- is easy to learn and use.

Please see the project page above for more information as well as the
mailing list for questions, discussions, and development.  There is
also an IRC channel on OFTC (irc.oftc.net) #gtest available.  Please
join us!

<<<<<<< HEAD
Please note that code under scripts/generator/ is from the [cppclean
project](http://code.google.com/p/cppclean/) and under the Apache
License, which is different from Google Mock's license.

Requirements for End Users
--------------------------
=======
### Requirements for End Users ###
>>>>>>> readme merging

Google Mock is implemented on top of the [Google Test C++ testing
framework](http://github.com/google/googletest/), and depends on it.
You must use the bundled version of Google Test when using Google Mock, or
you may get compiler/linker errors.

You can also easily configure Google Mock to work with another testing
framework of your choice; although it will still need Google Test as
an internal dependency.  Please read
<http://code.google.com/p/googlemock/wiki/ForDummies#Using_Google_Mock_with_Any_Testing_Framework>
for how to do it.

Google Mock depends on advanced C++ features and thus requires a more
modern compiler.  The following are needed to use Google Mock:

#### Linux Requirements ####

These are the base requirements to build and use Google Mock from a source
package (as described below):

  * GNU-compatible Make or "gmake"
  * POSIX-standard shell
  * POSIX(-2) Regular Expressions (regex.h)
  * C++98-standard-compliant compiler (e.g. GCC 3.4 or newer)

#### Windows Requirements ####

<<<<<<< HEAD
  * Microsoft Visual C++ 8.0 SP1 or newer
=======
  * Microsoft Visual C++ 7.1 or newer

#### Cygwin Requirements ####

  * Cygwin 1.5.25-14 or newer
>>>>>>> readme merging

#### Mac OS X Requirements ####

  * Mac OS X 10.4 Tiger or newer
  * Developer Tools Installed

<<<<<<< HEAD
Requirements for Contributors
-----------------------------
=======
Also, you'll need [CMake](http://www.cmake.org/ CMake) 2.6.4 or higher if
you want to build the samples using the provided CMake script, regardless
of the platform.

### Requirements for Contributors ###
>>>>>>> readme merging

We welcome patches.  If you plan to contribute a patch, you need to
build Google Mock and its own tests from an SVN checkout (described
below), which has further requirements:

  * Automake version 1.9 or newer
  * Autoconf version 2.59 or newer
  * Libtool / Libtoolize
  * Python version 2.3 or newer (for running some of the tests and
    re-generating certain source files from templates)

<<<<<<< HEAD
## Getting the Source ##

There are two primary ways of getting Google Mock's source code: you
can download a [stable source release](releases),
or directly check out the source from our Git repository.
The Git checkout requires a few extra steps and some extra software
packages on your system, but lets you track development and make
patches much more easily, so we highly encourage it.

### Git Checkout ###

<<<<<<< HEAD
To check out the master branch of Google Mock, run the following git command:
=======
Snapshots of Google Test's master branch can be downloaded directly with
GitHub's "Download ZIP" button on the main page.
>>>>>>> readme merging

    git clone https://github.com/google/googlemock.git

If you are using a \*nix system and plan to use the GNU Autotools build
system to build Google Mock (described below), you'll need to
configure it now.  Otherwise you are done with getting the source
files.

To prepare the Autotools build system, enter the target directory of
the checkout command you used ('gmock-svn') and proceed with the
following command:

    autoreconf -fvi

Once you have completed this step, you are ready to build the library.
Note that you should only need to complete this step once.  The
subsequent 'make' invocations will automatically re-generate the bits
of the build system that need to be changed.

If your system uses older versions of the autotools, the above command
will fail.  You may need to explicitly specify a version to use.  For
instance, if you have both GNU Automake 1.4 and 1.9 installed and
'automake' would invoke the 1.4, use instead:

    AUTOMAKE=automake-1.9 ACLOCAL=aclocal-1.9 autoreconf -fvi

Make sure you're using the same version of automake and aclocal.

=======
>>>>>>> readme merging
## Setting up the Build ##
=======

### Generic Build Instructions ###

#### Setup ####
>>>>>>> readme merging

To build Google Mock and your tests that use it, you need to tell your
build system where to find its headers and source files.  The exact
way to do it depends on which build system you use, and is usually
straightforward.

#### Build ####

This section shows how you can integrate Google Mock into your
existing build system.

Suppose you put Google Mock in directory ${GMOCK\_DIR} and Google Test
in ${GTEST\_DIR} (the latter is ${GMOCK\_DIR}/gtest by default).  To
build Google Mock, create a library build target (or a project as
called by Visual Studio and Xcode) to compile

    ${GTEST_DIR}/src/gtest-all.cc and ${GMOCK_DIR}/src/gmock-all.cc

with

    ${GTEST_DIR}/include and ${GMOCK_DIR}/include

in the system header search path, and

    ${GTEST_DIR} and ${GMOCK_DIR}

in the normal header search path.  Assuming a Linux-like system and gcc,
something like the following will do:

    g++ -isystem ${GTEST_DIR}/include -I${GTEST_DIR} \
        -isystem ${GMOCK_DIR}/include -I${GMOCK_DIR} \
        -pthread -c ${GTEST_DIR}/src/gtest-all.cc
    g++ -isystem ${GTEST_DIR}/include -I${GTEST_DIR} \
        -isystem ${GMOCK_DIR}/include -I${GMOCK_DIR} \
        -pthread -c ${GMOCK_DIR}/src/gmock-all.cc
    ar -rv libgmock.a gtest-all.o gmock-all.o

(We need -pthread as Google Test and Google Mock use threads.)

Next, you should compile your test source file with
${GTEST\_DIR}/include and ${GMOCK\_DIR}/include in the header search
path, and link it with gmock and any other necessary libraries:

    g++ -isystem ${GTEST_DIR}/include -isystem ${GMOCK_DIR}/include \
        -pthread path/to/your_test.cc libgmock.a -o your_test

As an example, the make/ directory contains a Makefile that you can
use to build Google Mock on systems where GNU make is available
(e.g. Linux, Mac OS X, and Cygwin).  It doesn't try to build Google
Mock's own tests.  Instead, it just builds the Google Mock library and
a sample test.  You can use it as a starting point for your own build
script.

If the default settings are correct for your environment, the
following commands should succeed:

    cd ${GMOCK_DIR}/make
    make
<<<<<<< HEAD
    ./gmock_test
=======
    ./sample1_unittest

If you see errors, try to tweak the contents of `make/Makefile` to make
them go away.  There are instructions in `make/Makefile` on how to do
it.

### Using CMake ###

Google Test comes with a CMake build script (
[CMakeLists.txt](CMakeLists.txt)) that can be used on a wide range of platforms ("C" stands for
cross-platform.). If you don't have CMake installed already, you can
download it for free from <http://www.cmake.org/>.

CMake works by generating native makefiles or build projects that can
be used in the compiler environment of your choice.  The typical
workflow starts with:

    mkdir mybuild       # Create a directory to hold the build output.
    cd mybuild
    cmake ${GTEST_DIR}  # Generate native build scripts.

If you want to build Google Test's samples, you should replace the
last command with
>>>>>>> update googletest readme to fix broken links

If you see errors, try to tweak the contents of [make/Makefile](make/Makefile) to make them go away.

### Windows ###

The msvc/2005 directory contains VC++ 2005 projects and the msvc/2010
directory contains VC++ 2010 projects for building Google Mock and
selected tests.

Change to the appropriate directory and run "msbuild gmock.sln" to
build the library and tests (or open the gmock.sln in the MSVC IDE).
If you want to create your own project to use with Google Mock, you'll
have to configure it to use the `gmock_config` propety sheet.  For that:

 * Open the Property Manager window (View | Other Windows | Property Manager)
 * Right-click on your project and select "Add Existing Property Sheet..."
 * Navigate to `gmock_config.vsprops` or `gmock_config.props` and select it.
 * In Project Properties | Configuration Properties | General | Additional
   Include Directories, type <path to Google Mock>/include.

<<<<<<< HEAD
Tweaking Google Mock
--------------------
=======
If you wish to use the Google Test Xcode project with Xcode 4.x and
above, you need to either:

 * update the SDK configuration options in xcode/Config/General.xconfig.
   Comment options `SDKROOT`, `MACOS_DEPLOYMENT_TARGET`, and `GCC_VERSION`. If
   you choose this route you lose the ability to target earlier versions
   of MacOS X.
 * Install an SDK for an earlier version. This doesn't appear to be
   supported by Apple, but has been reported to work
   (http://stackoverflow.com/questions/5378518).

### Tweaking Google Test ###
>>>>>>> readme merging

Google Mock can be used in diverse environments.  The default
configuration may not work (or may not work well) out of the box in
some environments.  However, you can easily tweak Google Mock by
defining control macros on the compiler command line.  Generally,
these macros are named like `GTEST_XYZ` and you define them to either 1
or 0 to enable or disable a certain feature.

We list the most frequently used macros below.  For a complete list,
<<<<<<< HEAD
see file [${GTEST\_DIR}/include/gtest/internal/gtest-port.h](
../googletest/include/gtest/internal/gtest-port.h).
=======
see file [include/gtest/internal/gtest-port.h](include/gtest/internal/gtest-port.h).
>>>>>>> update googletest readme to fix broken links

### Choosing a TR1 Tuple Library ###

Google Mock uses the C++ Technical Report 1 (TR1) tuple library
heavily.  Unfortunately TR1 tuple is not yet widely available with all
compilers.  The good news is that Google Test 1.4.0+ implements a
subset of TR1 tuple that's enough for Google Mock's need.  Google Mock
will automatically use that implementation when the compiler doesn't
provide TR1 tuple.

Usually you don't need to care about which tuple library Google Test
and Google Mock use.  However, if your project already uses TR1 tuple,
you need to tell Google Test and Google Mock to use the same TR1 tuple
library the rest of your project uses, or the two tuple
implementations will clash.  To do that, add

    -DGTEST_USE_OWN_TR1_TUPLE=0

to the compiler flags while compiling Google Test, Google Mock, and
your tests.  If you want to force Google Test and Google Mock to use
their own tuple library, just add

    -DGTEST_USE_OWN_TR1_TUPLE=1

to the compiler flags instead.

If you want to use Boost's TR1 tuple library with Google Mock, please
refer to the Boost website (http://www.boost.org/) for how to obtain
it and set it up.

### As a Shared Library (DLL) ###

Google Mock is compact, so most users can build and link it as a static
library for the simplicity.  Google Mock can be used as a DLL, but the
same DLL must contain Google Test as well.  See
[Google Test's README][gtest_readme]
for instructions on how to set up necessary compiler settings.

### Tweaking Google Mock ###

Most of Google Test's control macros apply to Google Mock as well.
Please see [Google Test's README][gtest_readme] for how to tweak them.

<<<<<<< HEAD
Upgrading from an Earlier Version
---------------------------------
=======
Note: while the above steps aren't technically necessary today when
using some compilers (e.g. GCC), they may become necessary in the
future, if we decide to improve the speed of loading the library (see
<http://gcc.gnu.org/wiki/Visibility> for details).  Therefore you are
recommended to always add the above flags when using Google Test as a
shared library.  Otherwise a future release of Google Test may break
your build script.

### Avoiding Macro Name Clashes ###

In C++, macros don't obey namespaces.  Therefore two libraries that
both define a macro of the same name will clash if you `#include` both
definitions.  In case a Google Test macro clashes with another
library, you can force Google Test to rename its macro to avoid the
conflict.

Specifically, if both Google Test and some other code define macro
FOO, you can add

    -DGTEST_DONT_DEFINE_FOO=1

to the compiler flags to tell Google Test to change the macro's name
from `FOO` to `GTEST_FOO`.  Currently `FOO` can be `FAIL`, `SUCCEED`,
or `TEST`.  For example, with `-DGTEST_DONT_DEFINE_TEST=1`, you'll
need to write

    GTEST_TEST(SomeTest, DoesThis) { ... }

instead of

    TEST(SomeTest, DoesThis) { ... }

in order to define a test.

<<<<<<< HEAD
### Upgrating from an Earlier Version ###
>>>>>>> readme merging

We strive to keep Google Mock releases backward compatible.
Sometimes, though, we have to make some breaking changes for the
users' long-term benefits.  This section describes what you'll need to
do if you are upgrading from an earlier version of Google Mock.

### Upgrading from 1.1.0 or Earlier ###

You may need to explicitly enable or disable Google Test's own TR1
tuple library.  See the instructions in section "[Choosing a TR1 Tuple
Library](../googletest/#choosing-a-tr1-tuple-library)".

### Upgrading from 1.4.0 or Earlier ###

On platforms where the pthread library is available, Google Test and
Google Mock use it in order to be thread-safe.  For this to work, you
may need to tweak your compiler and/or linker flags.  Please see the
"[Multi-threaded Tests](../googletest#multi-threaded-tests
)" section in file Google Test's README for what you may need to do.

If you have custom matchers defined using `MatcherInterface` or
`MakePolymorphicMatcher()`, you'll need to update their definitions to
use the new matcher API (
[monomorphic](http://code.google.com/p/googlemock/wiki/CookBook#Writing_New_Monomorphic_Matchers),
[polymorphic](http://code.google.com/p/googlemock/wiki/CookBook#Writing_New_Polymorphic_Matchers)).
Matchers defined using `MATCHER()` or `MATCHER_P*()` aren't affected.


<<<<<<< HEAD
Developing Google Mock
----------------------
=======
=======
>>>>>>> readme merging
## Developing Google Test ##
>>>>>>> readme merging

This section discusses how to make your own changes to Google Mock.

### Testing Google Mock Itself ###

To make sure your changes work as intended and don't break existing
functionality, you'll want to compile and run Google Test's own tests.
For that you'll need Autotools.  First, make sure you have followed
the instructions in section "SVN Checkout" to configure Google Mock.
Then, create a build output directory and enter it.  Next,

    ${GMOCK_DIR}/configure  # Standard GNU configure script, --help for more info

Once you have successfully configured Google Mock, the build steps are
standard for GNU-style OSS packages.

    make        # Standard makefile following GNU conventions
    make check  # Builds and runs all tests - all should pass.

Note that when building your project against Google Mock, you are building
against Google Test as well.  There is no need to configure Google Test
separately.

<<<<<<< HEAD
### Regenerating Source Files ###

Some of Google Mock's source files are generated from templates (not
in the C++ sense) using a script.  A template file is named FOO.pump,
where FOO is the name of the file it will generate.  For example, the
file `include/gmock/gmock-generated-actions.h.pump` is used to generate
`gmock-generated-actions.h` in the same directory.

Normally you don't need to worry about regenerating the source files,
unless you need to modify them.  In that case, you should modify the
corresponding `.pump` files instead and run the 'pump' script (for Pump
is Useful for Meta Programming) to regenerate them.  You can find
pump.py in the `${GTEST_DIR}/scripts/` directory.  Read the
[Pump manual](http://code.google.com/p/googletest/wiki/PumpManual)
for how to use it.


### Contributing a Patch ###

We welcome patches.  Please read the [Google Mock developer's Guide](
http://code.google.com/p/googlemock/wiki/DevGuide)
for how you can contribute.  In particular, make sure you have signed
the Contributor License Agreement, or we won't be able to accept the
patch.


Happy testing!

[gtest_readme]: ../googletest/ "googletest"
=======
Normally you don't need to worry about regenerating the source files,
unless you need to modify them.  In that case, you should modify the
corresponding .pump files instead and run the pump.py Python script to
regenerate them.  You can find pump.py in the [scripts/](scripts/) directory.
Read the [Pump manual](docs/PumpManual.md) for how to use it.
>>>>>>> readme merging
