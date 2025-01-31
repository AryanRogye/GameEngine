cmake_minimum_required(VERSION 3.10)
project(client)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

find_package(SDL2 REQUIRED)

include_directories(
        ${SDL2_INCLUDE_DIRS}
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/include/client
        ${CMAKE_SOURCE_DIR}/include/core
        ${CMAKE_SOURCE_DIR}/include/server
        ${CMAKE_SOURCE_DIR}/include/shared
)

set(SOURCES
    src/client/client.cpp
    src/client/packet.cpp
    src/shared/game.cpp
    src/shared/player.cpp
    src/main.cpp
)

add_executable(client
    ${SOURCES}
)

target_link_libraries(client
    ${SDL2_LIBRARIES}
)
