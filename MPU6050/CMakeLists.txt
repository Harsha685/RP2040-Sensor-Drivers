# Generated Cmake Pico project file
cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.2.0)
set(toolchainVersion 14_2_Rel1)
set(picotoolVersion 2.2.0-a4)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
set(PICO_BOARD pico_w CACHE STRING "Board type")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

project(MPU6050 C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# 1. Add your executable linking MPU6050.c and main.c together
add_executable(MPU6050 
    drivers/MPU6050.c
    main.c
)

pico_set_program_name(MPU6050 "MPU6050")
pico_set_program_version(MPU6050 "0.1")

# Enable USB serial output for terminal monitoring
pico_enable_stdio_uart(MPU6050 0)
pico_enable_stdio_usb(MPU6050 1)

# 2. Link all required hardware libraries (Added hardware_i2c and pico_cyw43_arch_none here)
target_link_libraries(MPU6050
    pico_stdlib
    hardware_i2c
    pico_cyw43_arch_none
)

# 3. Allow main.c to find MPU6050.h in the current directory
target_include_directories(MPU6050 PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/drivers
)

pico_add_extra_outputs(MPU6050)