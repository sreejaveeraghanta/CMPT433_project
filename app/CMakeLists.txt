# Build the app, using the HAL

include_directories(include)
file(GLOB MY_SOURCES "src/*.c")
add_executable(project ${MY_SOURCES})

# Make use of the libraries
target_link_libraries(project LINK_PRIVATE hal)
target_link_libraries(project LINK_PRIVATE lcd)
target_link_libraries(project LINK_PRIVATE lgpio)

# Copy executable to final location (change `hello_world` to project name as needed)
add_custom_command(TARGET project POST_BUILD 
  COMMAND "${CMAKE_COMMAND}" -E copy 
     "$<TARGET_FILE:project>"
     "~/cmpt433/public/project" 
  COMMENT "Copying ARM executable to public NFS directory")
