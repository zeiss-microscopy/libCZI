set(LIBCZI_PUBLIC_HDRS
        ImportExport.h
        libCZI_Compositor.h
        libCZI_DimCoordinate.h
        libCZI_exceptions.h
        libCZI_Helpers.h
        libCZI_Metadata.h
        libCZI_Pixels.h
        libCZI_Site.h
        libCZI_Utilities.h
        libCZI.h)

set(LIBCZI_SRCS
        bitmapData.h
        BitmapOperations.cpp
        BitmapOperations.h
        BitmapOperations.hpp
        CreateBitmap.cpp
        CziAttachment.cpp
        CziAttachment.h
        CziAttachmentsDirectory.cpp
        CziAttachmentsDirectory.h
        CziDataStructs.h
        CziDimensionInfo.cpp
        CziDimensionInfo.h
        CziDisplaySettings.cpp
        CziDisplaySettings.h
        CziMetadata.cpp
        CziMetadata.h
        CziMetadataDocumentInfo.cpp
        CziMetadataDocumentInfo.h
        CziMetadataSegment.cpp
        CziMetadataSegment.h
        CziParse.cpp
        CziParse.h
        CZIReader.cpp
        CZIReader.h
        CziStructs.h
        CziSubBlock.cpp
        CziSubBlock.h
        CziSubBlockDirectory.cpp
        CziSubBlockDirectory.h
        CziUtils.cpp
        CziUtils.h
        decoder.cpp
        decoder.h
        decoder_wic.cpp
        decoder_wic.h
        DimCoordinate.cpp
        dllmain.cpp
        IndexSet.cpp
        IndexSet.h
        libCZI_Lib.cpp
        libCZI_Site.cpp
        libCZI_Utilities.cpp
        MD5Sum.cpp
        MD5Sum.h
        MultiChannelCompositor.cpp
        MultiChannelCompositor.h
        priv_guiddef.h
        pugiconfig.hpp
        pugixml.cpp
        pugixml.hpp
        SingleChannelAccessorBase.cpp
        SingleChannelAccessorBase.h
        SingleChannelPyramidLevelTileAccessor.cpp
        SingleChannelPyramidLevelTileAccessor.h
        SingleChannelScalingTileAccessor.cpp
        SingleChannelScalingTileAccessor.h
        SingleChannelTileAccessor.cpp
        SingleChannelTileAccessor.h
        SingleChannelTileCompositor.cpp
        SingleChannelTileCompositor.h
        Site.h
        splines.cpp
        splines.h
        stdafx.cpp
        stdafx.h
        stdAllocator.cpp
        stdAllocator.h
        StreamImpl.cpp
        StreamImpl.h
        targetver.h
        utilities.cpp
        utilities.h)

file(GLOB_RECURSE LIBCZI_EIGEN_HDRS "eigen/*")

add_library(libCZI SHARED ${LIBCZI_PUBLIC_HDRS} ${LIBCZI_SRCS} ${LIBCZI_EIGEN_HDRS})

set_target_properties(libCZI PROPERTIES DEFINE_SYMBOL "LIBCZI_EXPORTS")
set_target_properties(libCZI PROPERTIES PUBLIC_HEADER "${LIBCZI_PUBLIC_HDRS}")

target_link_libraries (libCZI PRIVATE JxrDecodeStatic)

add_library(libCZIStatic STATIC ${LIBCZI_PUBLIC_HDRS} ${LIBCZI_SRCS} ${LIBCZI_EIGEN_HDRS})

set_target_properties(libCZIStatic PROPERTIES DEFINE_SYMBOL "LIBCZI_EXPORTS")
set_target_properties(libCZIStatic PROPERTIES PUBLIC_HEADER "${LIBCZI_PUBLIC_HDRS}")

install(TARGETS libCZI libCZIStatic
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        PUBLIC_HEADER DESTINATION include)
