
file(GLOB LIBCZISRCFILES "*.h" "*.cpp" "*.hpp")
file(GLOB_RECURSE LIBCZISRCEIGENFILES "eigen/*")
#add_library(NaCZIrLib SHARED CreateBitmap.cpp CziDimensionInfo.cpp CziMetadata.cpp CziMetadataDocumentInfo.cpp CziMetadataSegment.cpp CziParse.cpp CZIReader.cpp CziSubBlock.cpp CziSubBlockDirectory.cpp CziUtils.cpp decoder.cpp DimCoordinate.cpp dllmain.cpp MultiChannelTileCompositor.cpp NaCZIrLib.cpp NaCZIr_Utilities.cpp SingleChannelTileAccessor.cpp SingleChannelTileCompositor.cpp stdafx.cpp stdAllocator.cpp bitmapData.h CreateBitmap.h CziDataStructs.h CziDimensionInfo.h CziMetadata.h CziMetadataDocumentInfo.h CziMetadataSegment.h CziParse.h CZIReader.h CziStructs.h CziSubBlock.h CziSubBlockDirectory.h CziUtils.h datastore.h decoder.h DimCoordinate.h DimensionIndexExtended.h ImportExport.h MultiChannelTileCompositor.h NaCZIr.h NaCZIr_Compositor.h NaCZIr_DimCoordinate.h NaCZIr_Metadata.h NaCZIr_Pixels.h NaCZIr_Utilities.h readerwriterlocker.h SingleChannelTileAccessor.h SingleChannelTileCompositor.h stdafx.h stdAllocator.h targetver.h utilities.cpp utilities.h pugiconfig.hpp pugixml.cpp pugiconfig.hpp CziDisplaySettings.h CziDisplaySettings.cpp NaCZIr_Site.h NaCZIr_Site.cpp BitmapOperations.cpp BitmapOperations.h MD5Sum.cpp MD5Sum.h splines.h splines.cpp Site.h priv_guiddef.h NaCZIr_exceptions.h SingleChannelPyramidLevelTileAccessor.cpp SingleChannelPyramidLevelTileAccessor.h SingleChannelAccessorBase.cpp SingleChannelAccessorBase.h SingleChannelScalingTileAccessor.cpp SingleChannelScalingTileAccessor.h StreamImpl.cpp StreamImpl.h)

add_library(libCZI SHARED ${LIBCZISRCFILES} ${LIBCZISRCEIGENFILES})

SET_TARGET_PROPERTIES (libCZI PROPERTIES DEFINE_SYMBOL  "LIBCZI_EXPORTS" )

target_link_libraries (libCZI PRIVATE JxrDecodeStatic)

add_library(libCZIStatic STATIC ${LIBCZISRCFILES} ${LIBCZISRCEIGENFILES})

SET_TARGET_PROPERTIES (libCZIStatic PROPERTIES DEFINE_SYMBOL  "LIBCZI_EXPORTS" )
