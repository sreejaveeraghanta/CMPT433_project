cmake_minimum_required(VERSION 3.3)
project(BeagleYAI_Game)

include(GNUInstallDirs)

set(PVR_SDK_ROOT "$ENV{HOME}/powervr")
set(PVR_WINDOW_SYSTEM NullWS)

add_compile_options(-fdiagnostics-color -O3)

# Compile definitions, options that can be set  others
include("${PVR_SDK_ROOT}/Configuration.cmake")
include("${PVR_SDK_ROOT}/framework/FrameworkCommon.cmake")

####################################################
include(${PVR_SDK_ROOT}/cmake/utilities/executable.cmake)
include(${PVR_SDK_ROOT}/cmake/utilities/assets.cmake)
include(${PVR_SDK_ROOT}/cmake/utilities/spirv.cmake)
list(APPEND CMAKE_MODULE_PATH "${PVR_SDK_ROOT}/cmake/modules")
####################################################

# Make a top-level project file to build everything
add_library(PowerVR_SDK INTERFACE)

add_subdirectory(${PVR_SDK_ROOT}/external ${CMAKE_BINARY_DIR}/external EXCLUDE_FROM_ALL)
add_subdirectory(${PVR_SDK_ROOT}/framework ${CMAKE_BINARY_DIR}/framework EXCLUDE_FROM_ALL)

# Ignore warning from PVR SDK
get_directory_property(ALL_TARGETS DIRECTORY ${PVR_SDK_ROOT}/external BUILDSYSTEM_TARGETS)
foreach(target IN LISTS ALL_TARGETS)
    target_compile_options(${target} PRIVATE -w)
endforeach()
get_directory_property(ALL_TARGETS DIRECTORY ${PVR_SDK_ROOT}/framework BUILDSYSTEM_TARGETS)
foreach(target IN LISTS ALL_TARGETS)
    target_compile_options(${target} PRIVATE -w)
endforeach()

add_subdirectory(app)
add_subdirectory(vendor)
add_subdirectory(hal)
