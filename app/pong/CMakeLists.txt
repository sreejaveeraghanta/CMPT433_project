cmake_minimum_required(VERSION 3.3)
project(pong)

find_package(PVRShell REQUIRED MODULE)
find_package(PVRUtilsGles REQUIRED MODULE)

set(SRC_FILES
   src/game.cpp
   src/pongShell.cpp
   src/resourceManager.cpp
   src/shader.cpp    
   src/spriteRenderer.cpp
   src/texture.cpp
   src/util.cpp
)

add_executable(pong ${SRC_FILES})

target_include_directories(pong PRIVATE include)

# Suppress warnings from PVR SDK header
target_include_directories(pong SYSTEM PRIVATE
   ${PVR_SDK_ROOT}/framework
)

target_compile_options(pong PRIVATE -Wall -Wextra -Wpedantic)

target_link_libraries(pong PRIVATE
	PVRShell
	PVRUtilsGles
	hal
)

# Copy binary file
add_custom_command(TARGET pong POST_BUILD 
  COMMAND "${CMAKE_COMMAND}" -E copy 
     "$<TARGET_FILE:pong>"
     "~/cmpt433/public/myApps/pong/pong" 
  COMMENT "Copying ARM executable to public NFS directory")

# Copy assets
add_custom_command(TARGET pong POST_BUILD
 COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_SOURCE_DIR}/assets/pong"
    "~/cmpt433/public/myApps/pong/"
 COMMENT "Copying assets to public NFS directory")