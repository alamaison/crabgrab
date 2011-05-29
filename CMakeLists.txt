cmake_minimum_required(VERSION 2.6)
project(crabgrab)

include_directories(.)
include_directories(urdl/include)
include_directories(winapi/include)

set(Boost_USE_STATIC_LIBS TRUE)
set(Boost_DETAILED_FAILURE_MSG TRUE)
set(Boost_ADDITIONAL_VERSIONS 1.46.1)
find_package(Boost 1.45 REQUIRED COMPONENTS system)
link_directories(${Boost_LIBRARY_DIRS})
include_directories(${Boost_INCLUDE_DIRS})

find_package(OpenSSL REQUIRED)
include_directories(${OPENSSL_INCLUDE_DIR})

add_definitions(
  -DWIN32_LEAN_AND_MEAN -D_SCL_SECURE_NO_WARNINGS -DURDL_HEADER_ONLY=1
  -D_WIN32_WINNT=0x0501)

file(GLOB SOURCES *.cpp *.hpp *.c *.h)

add_executable(crabgrab ${SOURCES})
target_link_libraries(crabgrab ${OPENSSL_LIBRARIES})



