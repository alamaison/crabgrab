include_directories(.)
include_directories(urdl/include)
include_directories(winapi/include)

## Boost

set(Boost_USE_STATIC_LIBS TRUE)
set(Boost_DETAILED_FAILURE_MSG TRUE)
set(Boost_ADDITIONAL_VERSIONS 1.46.1)
# 1.41 is first version with Boost.PropertyTree
find_package(Boost 1.41 REQUIRED COMPONENTS system)
link_directories(${Boost_LIBRARY_DIRS})
include_directories(${Boost_INCLUDE_DIRS})

## OpenSSL

find_package(OpenSSL REQUIRED)
include_directories(${OPENSSL_INCLUDE_DIR})

## Preprocessor

add_definitions(
  -DWIN32_LEAN_AND_MEAN -D_SCL_SECURE_NO_WARNINGS -DURDL_HEADER_ONLY=1
  -D_WIN32_WINNT=0x0501)

## Sources

set(CRABGRAB_SOURCES
  crabgrab/convert_hbitmap.hpp
  crabgrab/encode_bmp.hpp
  crabgrab/encode_bmp.cpp
  crabgrab/main.cpp
  crabgrab/twitpic/generate_body.hpp
  crabgrab/twitpic/generate_body.cpp
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

## Build

add_executable(crabgrab ${SOURCES})
target_link_libraries(crabgrab ${OPENSSL_LIBRARIES})
