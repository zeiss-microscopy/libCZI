Building libCZI                 {#buildinglibCZI}
===============

libCZI aims to be portable and should build readily using a C++11 compiler. Here are some instructions
for building on Windows and on Linux.

Building on Windows in Visual Studio
------------------------------------

For Windows a solution-file (for VisualStudio 2017) <tt>LibCZI-Solution</tt> is provided.

![libCZI solution](Images/VisualStudioProj_1_a.PNG "libCZI solution")
@image latex VisualStudioProj_1.PNG "libCZI solution"

We find those solution configurations and solution platforms defined:

![libCZI solution configurations/platforms](Images/VisualStudioProj_2_a.PNG "libCZI solution configurations/platforms")
@image latex images/VisualStudioProj_2_a.PNG "libCZI solution configurations/platforms" height=10.8cm

The solution configurations are set up to build these targets into the folders listed here:

| Platform  | Configuration  | Folder                        |
|-----------|----------------|-------------------------------|
|Win32      | Debug          | ..\\Build\\VS\\Win32\\Debug   |
|Win32      | Release        | ..\\Build\\VS\\Win32\\Release |
|x64     	| Debug          | ..\\Build\\VS\\x64\\Debug     |
|x64        | Release        | ..\\Build\\VS\\x64\\Release   |

![libCZI Visual Studio project - build targets](Images/VisualStudio_BuildTargets_a.PNG "libCZI Visual Studio project - build targets")

The following files are found here:

|Filename             |Type                    |Description                   |
|---------------------|------------------------|------------------------------|
|CZIcmd.exe           | Executable             | commandline-executable of libCZI-sample-application (statically linked)       |
|JxrDecode[d].lib     | static library         | JPG-XR-codec (lib), with trailing 'd' for debug-build                        |
|libCZI[d].lib     	  | static library         | libCZI (lib), with trailing 'd' for debug-build                                     |
|liblibCZI.dll        | dynamic link library   | libCZI as DLL                                                                        |
|liblibCZI.lib        | import-library         | import-library for the dynamic link library liblibCZI.dll                          |
|libCZI_UnitTests.exe | Google-Test-Executable | Unit-Tests (with [Google-Test framework](https://github.com/google/googletest))   |

Note: VisualStudio 2017 allows to run the unit-test in Test-Explorer provided that the "Test Adapter for Google Test" is installed.

![libCZI Visual Studio project - unit tests](Images/VisualStudio_Proj_UnitTest_a.PNG "libCZI Visual Studio project - unit tests")

If linking with the static library, note that you need to link with libCZI[d].lib __and__ JxrDecode[d].lib.


Building with CMake
-------------------

libCZI can be build using  [CMake](https://cmake.org/) for which scripts are provided. It is recommended to use out-of-source builds (which is what is shown in the examples given here).

Suppose you put the the libCZI-source into the folder libCZIrw. Then create a folder beside this one for the out-of-source build, and cd into this folder.

    mkdir build_libczi
	cd build_libczi

Now we are ready to run CMake. Simply running the command
 
    cmake ../libCZIrw/

should give an output like this:
![running CMake](Images/cmake_1_raspi.PNG "running CMake")

Running <tt>make</tt> should now produce static and dynamic libraries, and also the CZIcmd-application and the unit-tests.

    sudo make install

will install the binaries and headers.
![make install](Images/cmake_install_raspi1.PNG "make install")

Notes:
* CMake does not create an **uninstall** target, in order to remove the files installed you may use the file *install_manifest.txt* (see also [here](https://stackoverflow.com/questions/41471620/cmake-support-make-uninstall)) like so:

    `sudo xargs rm < install_manifest.txt`

* The Build-Type should be specified with the variable `CMAKE_BUILD_TYPE` (see e.g. [here](https://stackoverflow.com/questions/7724569/debug-vs-release-in-cmake) or [here](https://cmake.org/cmake/help/latest/variable/CMAKE_BUILD_TYPE.html)), so for a release-build we have:

    `cmake -DBUILD_TYPE=Release  ../libCZIrw/`

* The CMake-script will download the [Google-Test-framework](https://github.com/google/googletest) from GitHub. If this is not desired (e.g. if there is no internet-connection), unit-tests may be excluded from the build by setting the variable `BUILD_UNITTESTS` to **OFF**:

    `cmake -DBUILD_UNITTESTS=OFF ../libCZIrw/`

    If the unit-tests have been built sucessfully, they can be run like this:
    ![running unit-tests](Images/cmake_raspi_unittests_1.PNG "running unit-tests")

* On a Unix-system, building CZIcmd requires the library [libpng](http://www.libpng.org/pub/png/libpng.html) to be installed and available. It should be available on most platforms with the corresponding package-manager, e. g.

    `sudo apt-get install zlib1g-dev`

* If available, CZIcmd will use [Freetype](https://www.freetype.org/). It may be installed with e. g.

    `sudo apt-get install libfreetype6-dev`
    
	Check by running the command

	`./CZIcmd --help=bitmapgen`

	in order to check whether Freetype was used in build.	
    ![help=bitmapgen](Images/cmake_1_raspi_help_freetype.PNG)

* If CMake can detect an installation of [Eigen3](http://eigen.tuxfamily.org/index.php?title=Main_Page) it will configure the build to use it. Otherwise, it will use the version that is bundled with libCZI (in the folder ../libCZI/eigen) which may be outdated.


Building the documentation
--------------------------

Executing <tt>doxygen</tt> will produce the documentation in this folder:

![libCZI doxygen](doxygen_1.PNG "libCZI doxygen")
@image latex doxygen_1.PNG "libCZI doxygen"

In order to build the documentation in PDF-format, go into the folder ../Build/Doxygen/latex and
execute <tt>make</tt>.\n
We had one issue with this: in order for the PDF-generation to succeed properly, we needed to make this
change: in the file ../Build/Doxygen/latex/refman.tex (which is generated by the doxygen-run) find a line

	\usepackage[utf8]{inputenc}

and change it into

	\usepackage[utf8x]{inputenc}

![fixing issue with PDF](doxygen_2.PNG "fixing issue with PDF")
@image latex doxygen_2.PNG "fixing issue with PDF"
