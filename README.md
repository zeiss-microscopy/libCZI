# libCZI
***Open Source Cross-Platform C++ library to read CZI image files***
---------------------------------------------------------------------

***libCZI*** is a library intended for providing read-only access to the information contained in CZI-documents.

It features:

* reading subblocks and get the content as a bitmap
* reading subblocks which are compressed with JPEG-XR
* works with tiled images and pyramid images
* composing multi-channel images with tinting and applying a gradation curve
* access metadata

In a nutshell, it offers (almost...) the same functionality as the 2D-viewer in ZEN - in terms of composing the image (including display-settings) and managing the data found in a CZI-file.

![Testdata Definition](images/czi_github.png)

The code is written in C++11 and (so far) has been successfully compiled with

* Visual Studio 2015 (Microsoft C++ v. 19.00.23506)
* GCC 5.2.1 (on Ubuntu Linux 4.2.0)
* Clang 3.4.1 (on FreeBSD 10.2)

It is intended to be easily portable to other platforms.
