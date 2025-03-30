cmake_minimum_required(VERSION 3.3)
project(pong)

find_package(PVRShell REQUIRED MODULE)
find_package(PVRUtilsGles REQUIRED MODULE)
find_package(ALSA REQUIRED)

set(SRC_FILES
   src/audioMixer.c
   src/ball.cpp
   src/game.cpp
   src/gameObject.cpp
   src/paddle.cpp
   src/player.cpp
   src/pongShell.cpp
   src/resourceManager.cpp
   src/shader.cpp    
   src/sounds.c
   src/spriteRenderer.cpp
   src/texture.cpp
   src/uiRenderer.cpp
   src/util.cpp
)

set_source_files_properties(
    src/audioMixer.c
    src/sounds.c
    PROPERTIES LANGUAGE C
)

add_executable(pong ${SRC_FILES})

target_include_directories(pong PRIVATE include)

# Suppress warnings from PVR SDK header
target_include_directories(pong SYSTEM PRIVATE
   ${PVR_SDK_ROOT}/framework
)

target_link_libraries(pong PRIVATE
	PVRShell
	PVRUtilsGles
	hal
   ALSA::ALSA
   # -fsanitize=address
)

# ---------------- NEW! ----------------
# ALSA support
#find_package(ALSA REQUIRED)
#target_link_libraries(pong LINK_PRIVATE asound)

target_compile_options(pong PRIVATE -Wall -Wextra -Wpedantic)
# target_compile_options(pong PRIVATE -fsanitize=address)

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

 # Copy the WAV folder to NFS
add_custom_command(TARGET pong POST_BUILD 
COMMAND "${CMAKE_COMMAND}" -E copy_directory
   "${CMAKE_SOURCE_DIR}/wave-files"
   "~/cmpt433/public/myApps/pong/wave-files" 
COMMENT "Copying WAVE files to public NFS directory")