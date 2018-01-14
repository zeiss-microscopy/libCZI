libCZI Documentation                 {#mainpage}
====================

libCZI is a library intended for providing read-only access to the information contained in CZI-documents.  

[comment]: # (Markdown magic -> ending a line with two spaces gives a linebreak!)

It features
- reading subblocks and get the content as a bitmap
- reading subblocks which are compressed with JPEG-XR
- works with tiled images and pyramid images
- composing multi-channel images with tinting and applying a gradation curve
- access metadata

In a nutshell, it offers (almost...) the same functionality as the 2D-viewer in ZEN - in terms of composing the image (including display-settings) and
managing the data found in a CZI-file.

![ZEN - 2D-viewer](ZEN_screenshot_1.PNG "ZEN_Screenshot_1")
@image latex ZEN_screenshot_1.PNG "ZEN - 2D-viewer"

The code is written in C++11 and (so far) has been successfully compiled with
- Visual Studio 2015 (Microsoft C++ v. 19.00.23506)
- GCC 5.2.1 (on Ubuntu Linux 4.2.0)
- Clang 3.4.1 (on FreeBSD 10.2)

It is intended to be easily portable to other platforms.   

It aims to be thread-safe (by most [definitions](https://en.wikipedia.org/wiki/Thread_safety)) - which isn't too surprising since it only allows read-only functionality.
The library itself does not leverage multithreading, but it is designed for being used in a multithreaded environment.


CZI in a nutshell
=================

Conceptionally a CZI-document can be described as a set of blobs (or blocks - in this context just a binary data structure defined only by its length), which are linked by a directory structure.

![CZI - conceptual overview](CZI_1.PNG "CZI_1")
@image latex CZI_1.PNG "CZI - conceptual overview"

At the start of the file, there is a distinguished data-structure ("the header") which (amongst other things) contains the offsets of three other special blocks: 

- the metadata block (contains all "per document" metadata in XML-format)
- sub-block directory (contains a list of all sub-blocks contained in the document and their respective position in the file)
- the attachment directory (contains a list of all attachment-blocks contained in the document and their respective position in the file)

With the term _sub-block_ we are referring to an entity which contains a 2-dimensional image (or "a bitmap"), some associated metadata in XML-format (which we
refer to as _sub-block metadata_) and (potentially) some other binary attachment (referred to as _sub-block attachment_).\n
Do not confuse the terms "sub-block metadata"↔"metadata" and "sub-block attachment"↔"attachment" in this regard.
Sub-blocks are identified by something like a coordinate - a list of dimensions and for each dimension a value.

Attachments can contain any sort of binary data (their content is not further defined on the file-format level). They are identified by a string. A naming convention is used to discover their content.

The identifier (or coordinate) of a sub-block can be grouped in two categories: a number of abstract dimensions (abstract in the sense that the number is not directly related to a spatial point) and
an X-Y-coordinate in a 2D-plane. Examples for the former are "C-dimension" (used for different channels) or "T-dimensions" (used for images acquired at different points in time) → [link_enum_dimensionindex].
The X-Y-coordinate refer to a common (to all sub-blocks) coordinate system in which the sub-blocks are thought to be arranged. In fact - sub-blocks are not only described by their X-Y-coordinate,
but by a complete rectangle (adding witdth and height to the X-Y-coordinate) - called the _logical rect_. And, in addition there is a parameter _physical width_ and _physical height_ → [link_subblockinfo].\n

![CZI - position of sub-block on plane](CZI_2.PNG "CZI_2")
@image latex CZI_2.PNG "CZI - position of sub-block on plane"

So the logical_rect defines the position (and size) of the sub-block on a plane. The physical_size gives the width and height of the bitmap contained in the sub-block. They may or may not 
coincide with each other. If they do not coincide, then conceptually the bitmap is to be considered to have a different resolution - so that we do not have a one-to-one correspondence
between a pixel and an increment by 1 on the X- or Y-coordinate. This concept is leveraged in order to have a resolution pyramid available in a CZI-document.

[link_enum_dimensionindex]: @ref libCZI::DimensionIndex "cf. DimensionIndex"
[link_subblockinfo]: @ref libCZI::SubBlockInfo "cf. SubBlockInfo"