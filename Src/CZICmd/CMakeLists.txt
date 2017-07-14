IF(UNIX)
find_package(ZLIB)
find_package(PNG)
ENDIF(UNIX)

add_executable(CZIcmd cmdlineoptions.cpp consoleio.cpp execute.cpp getOpt.cpp CZIcmd.cpp SaveBitmap.cpp stdafx.cpp streamimpl.cpp cmdlineoptions.h consoleio.h execute.h getOpt.h inc_libCZI.h SaveBitmap.h stdafx.h streamimpl.h targetver.h utils.cpp utils.h platform_defines.h DisplaySettingsHelper.h)

add_definitions(-DUNICODE -D_UNICODE)
target_link_libraries(CZIcmd PRIVATE libCZI)
IF(UNIX)
target_link_libraries(CZIcmd ${PNG_LIBRARIES} ${ZLIB_LIBRARIES})
include_directories(${ZLIB_INCLUDE_DIR} ${PNG_INCLUDE_DIR})
ENDIF(UNIX)


