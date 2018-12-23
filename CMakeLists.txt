cmake_minimum_required(VERSION 3.0.0)
project(Flows)

# include directories
include_directories(include)

# add compiler flags
set(CXX_FLAGS "--std=c++17")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CXX_FLAGS}")

# add files
file(GLOB FILES "src/*.cpp")

# create executable
add_library(Flows SHARED ${FILES})