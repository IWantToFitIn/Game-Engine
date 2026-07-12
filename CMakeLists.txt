cmake_minimum_required(VERSION 3.15)
project(idk VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
# for any generated files
set(GEN_OUT_DIR "${CMAKE_SOURCE_DIR}/gen")

include(FetchContent)
set(SPIRV_REFLECT_INSTALL OFF)
set(SPIRV_REFLECT_EXECUTABLE OFF)
set(SPIRV_REFLECT_STATIC_LIB ON)
FetchContent_Declare(
	spirv-reflect
	GIT_REPOSITORY https://github.com/KhronosGroup/SPIRV-Reflect
	GIT_TAG main
	OVERRIDE_FIND_PACKAGE
)

if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

add_subdirectory(src)