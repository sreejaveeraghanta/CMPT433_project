cmake_minimum_required(VERSION 3.3)
project(pvrTriangle)

find_package(PVRShell REQUIRED MODULE)
find_package(PVRUtilsGles REQUIRED MODULE)

set(SRC_FILES src/triangle.cpp)
add_executable(pvrTriangle ${SRC_FILES})

target_link_libraries(pvrTriangle PUBLIC
	PVRShell
	PVRUtilsGles
	hal
	util
)

# Copy binary file
add_custom_command(TARGET pvrTriangle POST_BUILD 
  COMMAND "${CMAKE_COMMAND}" -E copy 
     "$<TARGET_FILE:pvrTriangle>"
     "~/cmpt433/public/myApps/pvrTriangle/pvrTriangle" 
  COMMENT "Copying ARM executable to public NFS directory")