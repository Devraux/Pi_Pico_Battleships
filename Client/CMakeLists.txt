cmake_minimum_required(VERSION 3.25)


# Select board:
set(PICO_BOARD pico_w)

include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)
include($ENV{PICO_SDK_PATH}/tools/CMakeLists.txt)

project(main C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

pico_sdk_init()

add_executable(${PROJECT_NAME}
    main.c
    Battleship/battleship.c
    Battleship/battleship_gameLoop.c
    UDP/udp.c
)

include_directories(${PROJECT_NAME}
    .
    Battleship
    UDP
    ../Utility
)


pico_add_extra_outputs(${PROJECT_NAME})


target_link_libraries(${PROJECT_NAME}
    pico_stdlib
    hardware_gpio
    pico_cyw43_arch_lwip_threadsafe_background
)

pico_enable_stdio_usb(${PROJECT_NAME}  1)
pico_enable_stdio_uart(${PROJECT_NAME} 1)
