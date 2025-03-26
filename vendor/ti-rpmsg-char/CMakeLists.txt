set(SRC_FILES
   src/rpmsg_char.c
   src/soc.c
   src/utils.c
)

add_library(ti_rpmsg_char STATIC ${SRC_FILES})

target_include_directories(ti_rpmsg_char PUBLIC include)
