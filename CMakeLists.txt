

cmake_minimum_required (VERSION 3.1.4)

project ("CarSharing")

message(STATUS "Setting MSVC flags")
set(CMAKE_CXX_STANDARD 14)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/../bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/../bin")


add_subdirectory (lib)
add_subdirectory (Server)
add_subdirectory (Client)
add_subdirectory (tests)


