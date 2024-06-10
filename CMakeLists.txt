# Specify the minimum version for CMake
cmake_minimum_required(VERSION 3.20)

# Project's name
project(ncmpp)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Add the directories where the header files are located
include_directories(ncmpp/include)
include_directories(ncmlib/include)

# Add the source files
file(GLOB NCMPP_SRC "ncmpp/src/*.cpp")
file(GLOB NCMLIB_SRC "ncmlib/src/*.cpp")

# Create the library
add_library(ncmlib ${NCMLIB_SRC})

# Create the executable
add_executable(ncmpp ${NCMPP_SRC})

# Link the libraries
target_link_libraries(ncmpp ncmlib)
target_link_libraries(ncmpp ssl)
target_link_libraries(ncmpp crypto)
if(WIN32)
    link_directories(${CMAKE_SOURCE_DIR}/ncmlib/ext/lib)
endif()
