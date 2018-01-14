Image Document Concept                 {#imagedocumentconcept}
======================

## General concepts ##

The sub-blocks contained in a CZI-file are conceptually organized as follows:

- Sub-blocks reside in different "planes", where a plane is given by the discrete coordinates 'Z', 'C', 'T' or 'V'.
- Each sub-block has an X-Y coordinate (and a width and height) in a 2D-coordinate system (which is common to all planes).
- Sub-blocks contain images which can be thought to fill an axis-aligned rectangle (specified by its X and Y coordinate, and its width and height).
- In addition, a sub-block may have a logical size which is different from its physical size (also called a "zoom").


![sub-blocks on a plane](image_document_concept1.PNG "image_document_concept1")
@image latex image_document_concept1.PNG "sub-blocks on a plane"

The case where we have different planes in one document is depicted here:

![sub-blocks on different planes](image_document_concept2.PNG "image_document_concept2")
@image latex image_document_concept2.PNG "sub-blocks on different planes"

Note that:
- The X-Y positions of sub-blocks on different planes can be different (i. e. same Z-index, same T-index and same M-index does not imply that X and Y is the same for all C-indices). Even the number of sub-blocks on different planes can be different.
- The bounding box is defined to contain all sub-blocks on all planes.
- The 2D-coordinate system is common to all planes.
- Sub-blocks can be overlapping.

## Dimensions ##

Each sub-blocks is labeled by a set of coordinates in different dimensions (the term "dimension" is used very loosely in the following discussion).  
We have already met the dimensions 'Z', 'C', 'T' and 'V' which are used to label different planes. In addition to them, a couple of more dimensions
are in use:

dimension   |  meaning     | comment
----------- | ------------ | ---------------------------------------------------------------------
 Z          | z-focus      | plane is from a different Z-plane 
 C          | channel      | different modality 
 T          | time         | different point in time 
 H          | phase        | distinguishes the different phases in a SIM-acquisition (structured illumination microscopy)
 I          | illumination | different directions of illumination (used in SPIM-acquisition)
 V          | view         | used in SPIM for different views

'H' and 'I' have the character of an attribute in the sense that sub-blocks which differ only in the 'H' (or 'I') coordinate must have the same X-Y-position
in order to be meaningfull.

There is also the letter 'M' in use for a dimension, but it has a somewhat different meaning. It is used in order to enumerate all tiles in a plane. I. e. all planes in a given plane
shall have an M-index, and this M-index starts counting from zero to the number of tiles on that plane. The counting starts from zero for all different planes (and scenes). Tiles 
from different planes which differ in C are expected to have the same M-index (and, usually have the same X-Y-coordinate, but there are cases where the X-Y-coordinates are not exactly
identical).

And we have the letter 'S' in use, and it is used in the following way: sub-blocks with the same S-index form a set called "scene". A scene is
a rectangular (and axis aligned) region, and much like the bounding box, it is determined by taking all planes into consideration.


![concept of scenes](image_document_concept3.PNG "image_document_concept3")
@image latex image_document_concept3.PNG "concept of scenes"

One restriction applies to scenes: scenes **may** overlap, but sub-blocks (on pyramid-layer 0) belonging to different scenes **must not** overlap.

![sub-block from different scenes must not overlap](image_document_concept4.PNG "image_document_concept4")
@image latex image_document_concept4.PNG "sub-block from different scenes must not overlap"



