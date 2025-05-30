# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(PICO_BOARD pico_w CACHE STRING "Board type")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

project(Smart_Switching C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Add executable. Default name is the project name, version 0.1

add_executable(Smart_Switching Smart_Switching.c lib/ssd1306.c)

pico_set_program_name(Smart_Switching "Smart_Switching")
pico_set_program_version(Smart_Switching "0.1")

# Modify the below lines to enable/disable output over UART/USB
pico_enable_stdio_uart(Smart_Switching 1)
pico_enable_stdio_usb(Smart_Switching 1)


pico_generate_pio_header(Smart_Switching ${CMAKE_CURRENT_LIST_DIR}/Smart_Switching.pio)
target_sources(Smart_Switching PRIVATE Smart_Switching.c)

# Add the standard library to the build
target_link_libraries(Smart_Switching
        pico_stdlib)

# Add the standard include files to the build
target_include_directories(Smart_Switching PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}
)

# Add any user requested libraries
target_link_libraries(Smart_Switching
                    hardware_adc
                    pico_stdlib
                    hardware_i2c
                    hardware_pwm
                    hardware_pio
                    hardware_timer  
        )

pico_add_extra_outputs(Smart_Switching)

