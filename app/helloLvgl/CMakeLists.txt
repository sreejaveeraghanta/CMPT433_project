set(SRC_FILES src/main.cpp)
add_executable(helloLvgl ${SRC_FILES})

target_link_libraries(helloLvgl PUBLIC
	hal
   lvgl::lvgl
   # -fsanitize=address
)

target_compile_options(helloLvgl PRIVATE -Wall -Wextra -Wpedantic -g -O0)
# target_compile_options(helloLvgl PRIVATE -fsanitize=address)

# Copy binary file
add_custom_command(TARGET helloLvgl POST_BUILD 
  COMMAND "${CMAKE_COMMAND}" -E copy 
     "$<TARGET_FILE:helloLvgl>"
     "~/cmpt433/public/myApps/helloLvgl/helloLvgl" 
  COMMENT "Copying ARM executable to public NFS directory")
