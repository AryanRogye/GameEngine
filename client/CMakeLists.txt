cmake_minimum_required(VERSION 3.10)
project(client)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(SDL2_DIR "/opt/homebrew/opt/sdl2/lib/cmake/SDL2")
find_package(SDL2 REQUIRED)
set(SDL2_IMAGE_DIR "/opt/homebrew/opt/sdl2_image/lib/cmake/SDL2_image")
find_package(SDL2_image REQUIRED)

add_executable(client
    src/client/client.cpp
    src/client/packet.cpp
    src/shared/game.cpp
    src/shared/player.cpp
    src/core/map_loader.cpp
    src/core/block.cpp
    src/main.cpp
)

target_include_directories(client PRIVATE
        ${SDL2_INCLUDE_DIRS}
        ${SDL2_IMAGE_INCLUDE_DIRS}
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/include/client
        ${CMAKE_SOURCE_DIR}/include/core
        ${CMAKE_SOURCE_DIR}/include/server
        ${CMAKE_SOURCE_DIR}/include/shared
)

target_link_libraries(client PRIVATE
    SDL2::SDL2
    SDL2_image::SDL2_image
    # ${SDL2_LIBRARIES}
    # ${SDL2_IMAGE_LIBRARIES}
)
