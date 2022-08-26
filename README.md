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

In a nutshell, it offers (almost...) the same functionality as the 2D-viewer in ZEN - in terms of composing the image (including display-settings) and managing the data found in a CZI-file. The libCZI libary is part of the Open Application Development Concept of Zeiss Microscopy. More information on OAD can be found here: https://github.com/zeiss-microscopy/OAD

![Testdata Definition](images/czi_github.png)

The code is written in C++11 and (so far) has been successfully compiled with:

* Visual Studio 2015 (Microsoft C++ v. 19.00.23506)
* GCC 5.2.1 (on Ubuntu Linux 4.2.0)
* Clang 3.4.1 (on FreeBSD 10.2)

It is intended to be easily portable to other platforms.

---------------------------------------------------------------------------------

***Licensing***

******************************************************************************
 
libCZI is a reader for the CZI fileformat written in C++
Copyright (C) 2017  Zeiss Microscopy GmbH
 
This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.
 
This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
 
You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
 
**To obtain a commercial version please contact Zeiss Microscopy GmbH.**
 
***

### Remarks

This repository will be archived soon because libCZI will be replaced vy the newer libCZIrw C++ library. It will be published on Github very soon (Summer 2022). This library contains important improvements and new features:

* allows to read and write CZI image files
* ZSTD compression
* new licensing LGPLv3


### *Python Wrapper for libCZI*

### pylibCZIrw

ZEISS has published a python wrapper for the new libCZIrw which can be found here: [pylibCZIrw](https://pypi.org/project/pylibCZIrw/)

### Community-driven tools to read CZIs

There are several available based on libCZI. They provide easy access to CZI image data from python but do not allow to write CZI files. Those wrappers were created by the community and therefore is by no means responsible for their functionality and their maintenance. Feel free to use them at your own risk.

#### aicspylibczi

The [aicspylibczirw](https://github.com/AllenCellModeling/aicspylibczi) library from the Allen Institute for Cell Science is based [pylibCZI](https://github.com/elhuhdron/pylibczi) and can be used on its own or as part of [AICSimageIO](https://allencellmodeling.github.io/aicsimageio/)

#### pylibCZI

This is a python module for reading Zeiss CZI files by utilizing libCZI and can be found here: [pylibCZI](https://github.com/elhuhdron/pylibczi)

#### czifile

The excellent library [czifile](https://pypi.org/project/czifile/) to read CZI file written in pure python. It seems not to be active anymore since over two years(2022-08-23). 