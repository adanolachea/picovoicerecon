cmake_minimum_required(VERSION 3.13)
include(pico_sdk_import.cmake)
project(ssd1306_i2c)
pico_sdk_init()

add_executable(ssd1306_i2c
        main.c
        drivers/ssd1306/ssd1306.c
        )


# pull in common dependencies and additional i2c hardware support
target_link_libraries(ssd1306_i2c pico_stdlib hardware_i2c)

# enable usb output, disable uart output
pico_enable_stdio_usb(ssd1306_i2c 1)
pico_enable_stdio_uart(ssd1306_i2c 0)

# create map/bin/hex file etc.
pico_add_extra_outputs(ssd1306_i2c)

# add url via pico_set_program_url
#example_auto_set_url(ssd1306_i2c)
