cmake_minimum_required(VERSION 2.6)

find_package(
  Boost REQUIRED
  COMPONENTS system unit_test_framework)

file(GLOB TEST_SOURCES *.cpp *.hpp)
file(GLOB PLATFORM_SPECIFIC_TEST_SOURCES ${PLATFORM}/*.cpp ${PLATFORM}/*.hpp)

add_executable(tests ${TEST_SOURCES} ${PLATFORM_SPECIFIC_TEST_SOURCES})
target_link_libraries(tests crabgrab-lib)
get_target_property(test_EXE tests LOCATION)

set(TEST_RUNNER_ARGUMENTS
  --catch_system_errors --detect_memory_leaks
  --result_code=no --log_level=test_suite)

add_custom_target(RUN_TESTS
  ${test_EXE} ${TEST_RUNNER_ARGUMENTS} DEPENDS tests)
