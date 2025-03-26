cmake_minimum_required(VERSION 3.3)
project(breakout)

find_package(PVRShell REQUIRED MODULE)
find_package(PVRUtilsGles REQUIRED MODULE)

set(SRC_FILES
   src/breakoutShell.cpp
   src/util.cpp
   src/game.cpp
   src/resourceManager.cpp
   src/shader.cpp
   src/texture.cpp
   src/spriteRenderer.cpp
   src/gameLevel.cpp
   src/gameObject.cpp
   src/ballObject.cpp
   src/particleGenerator.cpp
   src/postProcessor.cpp
)

add_executable(breakout ${SRC_FILES})

target_include_directories(breakout PRIVATE include)

# Suppress warnings from PVR SDK header
target_include_directories(breakout SYSTEM PRIVATE
   ${PVR_SDK_ROOT}/framework
)

target_compile_options(breakout PRIVATE -Wall -Wextra -Wpedantic)

target_link_libraries(breakout PRIVATE
	PVRShell
	PVRUtilsGles
	hal
)

# Copy binary file
add_custom_command(TARGET breakout POST_BUILD 
  COMMAND "${CMAKE_COMMAND}" -E copy 
     "$<TARGET_FILE:breakout>"
     "~/cmpt433/public/myApps/breakout/breakout" 
  COMMENT "Copying ARM executable to public NFS directory")

# Copy assets
add_custom_command(TARGET breakout POST_BUILD
 COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_SOURCE_DIR}/assets/breakout"
    "~/cmpt433/public/myApps/breakout/"
 COMMENT "Copying assets to public NFS directory")