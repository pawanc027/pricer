cmake_minimum_required(VERSION 3.10)

# set the project name
project(StkPricer)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Specify Flags
set(CMAKE_CXX_FLAGS "-Werror -Wall -Wextra -pedantic -pedantic-errors -Wconversion -Wsign-conversion")

# Add the executable
add_executable(StkPricer src/main.cpp src/pricer.cpp)

# I../includes
include_directories(inc)

target_link_libraries(StkPricer pthread)

set_target_properties( StkPricer
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)
