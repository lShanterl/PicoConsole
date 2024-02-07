cmake_minimum_required(VERSION 3.20)

include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

project(PicoConsole C CXX ASM)

set(PICO_BOARD pico_w)

pico_sdk_init()

add_executable(${PROJECT_NAME} main.cpp
        src/flappy_bird.cpp
        include/console_audio.hpp)

set(SOURCES
    src/console_core.cpp
    src/console_draw.cpp
    src/console_init.cpp
    src/console_misc.cpp
    src/console_audio.cpp
    src/flappy_bird.cpp
    src/wolfenstein.cpp
)

target_sources(${PROJECT_NAME} PRIVATE ${SOURCES})

set(HEADERS
    include/console.hpp
    include/sprites.hpp
    include/types.hpp
    include/console_sprites.hpp
    include/game_list.hpp
    include/console_audio.hpp
)

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_LIST_DIR}/include ${CMAKE_CURRENT_LIST_DIR}/src)

target_link_libraries(${PROJECT_NAME} 
    pico_stdlib
    pico_cyw43_arch_none
    hardware_pwm
    hardware_adc
    hardware_i2c
    hardware_spi
    pico_multicore
    pico_binary_info
	 
    # pico_cyw43_arch_lwip_threadsafe_background
)

pico_enable_stdio_usb(${PROJECT_NAME} 1)
pico_enable_stdio_uart(${PROJECT_NAME} 0)

pico_add_extra_outputs(${PROJECT_NAME})