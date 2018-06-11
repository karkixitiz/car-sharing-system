cmake_minimum_required (VERSION 3.1.1)

project (CarSharing)

# output directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# executable name
set(EXEC_NAME "carSharing")

add_executable(${EXEC_NAME} main.cpp)
set_property(TARGET ${EXEC_NAME} PROPERTY CXX_STANDARD 11)
set_property(TARGET ${EXEC_NAME} PROPERTY CXX_STANDARD_REQUIRED true)