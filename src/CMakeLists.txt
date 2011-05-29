## Sources

set(CRABGRAB_SOURCES
  crabgrab/clipboard.hpp
  crabgrab/convert_hbitmap.hpp
  crabgrab/encode_bmp.hpp
  crabgrab/encode_bmp.cpp
  crabgrab/twitpic/generate_body.hpp
  crabgrab/twitpic/generate_body.cpp
  crabgrab/twitpic/response.hpp
  crabgrab/twitpic/response.cpp
  crabgrab/twitpic/twitpic.hpp
  crabgrab/twitpic/twitpic.cpp
)

set(LODEPNG_SOURCES
  LodePNG/lodepng.h
  LodePNG/lodepng.cpp
)

set(BITMAP_SOURCES
  bitmap.h
)

set(SOURCES ${CRABGRAB_SOURCES} ${LODEPNG_SOURCES} ${BITMAP_SOURCES})

# This bit just prettyfies the source hierarchy in Visual Studio
macro(group_by_hierarchy PROJECT_SOURCES)
  foreach(SOURCE ${PROJECT_SOURCES})
    string(REGEX REPLACE "[\\\\/][^\\\\/]*$" "" GROUP_NAME "${SOURCE}" )
    string(REGEX REPLACE "^[\\\\/]" "" GROUP_NAME "${GROUP_NAME}" )
    string(REGEX REPLACE "/" "\\\\\\\\" GROUP_NAME "${GROUP_NAME}" )

#    if(${SOURCE} MATCHES "\\.h(pp)?$")
#      source_group("${GROUP_NAME}\\Header Files" FILES ${SOURCE})
#    elseif(${SOURCE} MATCHES "\\.c(pp)?$")
#      source_group("${GROUP_NAME}\\Source Files" FILES ${SOURCE})
#    else()
      source_group("${GROUP_NAME}" FILES ${SOURCE})
#    endif()
  endforeach()
endmacro()

group_by_hierarchy("${CRABGRAB_SOURCES}") # must be quoted to pass list
group_by_hierarchy("${LODEPNG_SOURCES}") # must be quoted to pass list

## OpenSSL

find_package(OpenSSL REQUIRED)
include_directories(${OPENSSL_INCLUDE_DIR})

## Build

add_library(crabgrab-lib ${SOURCES})
add_executable(crabgrab crabgrab/main.cpp)
target_link_libraries(crabgrab crabgrab-lib ${OPENSSL_LIBRARIES})
