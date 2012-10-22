project(luola2 C CXX)

cmake_minimum_required(VERSION 2.4 FATAL_ERROR)
set(
	CMAKE_MODULE_PATH
	${CMAKE_CURRENT_SOURCE_DIR}/config
	${CMAKE_MODULE_PATH}
)

include(CheckCXXCompilerFlag)

# Options

option(DEBUG "Enable debugging and asserts" OFF)

# Build type
if(DEBUG)
	set (CMAKE_BUILD_TYPE Debug)
else()
	set(CMAKE_BUILD_TYPE Release)
endif()

message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

# Compiler flags

check_cxx_compiler_flag(-std=c++11 HAS_CXX11)
if(HAS_CXX11)
	list(APPEND CMAKE_CXX_FLAGS "-std=c++11 -W -Wall -pedantic")
else()
	list(APPEND CMAKE_CXX_FLAGS "-std=c++0x -W -Wall -pedantic")
endif()

# Libraries
find_package(Boost COMPONENTS system filesystem program_options thread REQUIRED)
find_package(GLFW REQUIRED)
find_package(GLEW REQUIRED)
find_package(PNG REQUIRED)
find_package(MiniZip REQUIRED)

# Source
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin)
add_subdirectory(src/)

