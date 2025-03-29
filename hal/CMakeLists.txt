set(SRC_FILES
   src/gpiorw.c
   src/gpio.c
   src/i2c.c
   src/joystick.c
   src/lcd.c
   src/r5f.c
   src/spi.c
   src/pwm.c
   src/udp_client.c
)

add_library(hal STATIC ${SRC_FILES})

target_include_directories(hal PUBLIC include)

target_compile_options(hal PRIVATE -Wall -Wextra -Wpedantic)

target_link_libraries(
    hal LINK_PRIVATE ti_rpmsg_char util gpiod
)
