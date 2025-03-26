cmake_minimum_required(VERSION 3.3)
project(pvrGnomeToy)

find_package(PVRShell REQUIRED MODULE)
find_package(PVRUtilsGles REQUIRED MODULE)

set(SRC_FILES src/gnomeToy.cpp)
add_executable(pvrGnomeToy ${SRC_FILES})

target_link_libraries(pvrGnomeToy PUBLIC
	PVRShell
	PVRUtilsGles
	hal
)

# Copy binary file
add_custom_command(TARGET pvrGnomeToy POST_BUILD 
  COMMAND "${CMAKE_COMMAND}" -E copy 
     "$<TARGET_FILE:pvrGnomeToy>"
     "~/cmpt433/public/myApps/pvrGnomeToy/pvrGnomeToy" 
  COMMENT "Copying ARM executable to public NFS directory")

# Copy assets
add_custom_command(TARGET pvrGnomeToy POST_BUILD
   COMMAND ${CMAKE_COMMAND} -E copy_directory
      "${CMAKE_SOURCE_DIR}/assets/pvrGnomeToy"
      "~/cmpt433/public/myApps/pvrGnomeToy/"
   COMMENT "Copying assets to public NFS directory")