cmake_minimum_required(VERSION 3.25)


project(main C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

add_executable(${PROJECT_NAME}
    main.c
    Battleship/battleship.c
)

include_directories(${PROJECT_NAME}
    .
    Battleship
)


