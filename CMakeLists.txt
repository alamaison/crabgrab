cmake_minimum_required(VERSION 2.6)
project(crabgrab)

include_directories(src)
include_directories(src/urdl/include)
include_directories(src/winapi/include)
include_directories(src/comet/include)

## Boost

set(Boost_USE_STATIC_LIBS TRUE)
set(Boost_DETAILED_FAILURE_MSG TRUE)
set(Boost_ADDITIONAL_VERSIONS 1.46.1)
# 1.41 is first version with Boost.PropertyTree
find_package(Boost 1.41 REQUIRED COMPONENTS system)
link_directories(${Boost_LIBRARY_DIRS})
include_directories(${Boost_INCLUDE_DIRS})

## Preprocessor

add_definitions(
  -DWIN32_LEAN_AND_MEAN -D_SCL_SECURE_NO_WARNINGS -DURDL_HEADER_ONLY=1
  -D_WIN32_WINNT=0x0501 -DBOOST_FILESYSTEM_VERSION=2)

## Build

add_subdirectory(src)
# TODO: Make building the tests optional
add_subdirectory(test)
