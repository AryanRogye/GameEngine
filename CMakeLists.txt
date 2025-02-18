cmake_minimum_required(VERSION 3.10)
project(client)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(SDL2_DIR "/opt/homebrew/opt/sdl2/lib/cmake/SDL2")
find_package(SDL2 REQUIRED)
set(SDL2_IMAGE_DIR "/opt/homebrew/opt/sdl2_image/lib/cmake/SDL2_image")
find_package(SDL2_image REQUIRED)
# Include ENet
include_directories(/opt/homebrew/include)
link_directories(/opt/homebrew/lib)


add_executable(client
    src/shared/game.cpp
    src/shared/start_game.cpp
    src/shared/player.cpp
    src/shared/commands.cpp

    src/core/map_loader.cpp
    src/core/block.cpp
    src/core/world.cpp

    src/rendering/texture.cpp
    src/rendering/sprite.cpp
    src/rendering/ui.cpp

    src/configs.cpp
    src/main.cpp
)

target_include_directories(client PRIVATE
        ${SDL2_INCLUDE_DIRS}
        ${SDL2_IMAGE_INCLUDE_DIRS}
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/include/core
        ${CMAKE_SOURCE_DIR}/include/server
        ${CMAKE_SOURCE_DIR}/include/shared
        ${CMAKE_SOURCE_DIR}/include/rendering
)

target_link_libraries(client PRIVATE
    SDL2::SDL2
    SDL2_image::SDL2_image
    enet
)

# This Is The SandBox Shared Library
add_library(sandbox SHARED 
    src/sandbox/sandbox.cpp
    src/shared/player.cpp
    src/core/map_loader.cpp
    src/rendering/texture.cpp
    src/rendering/sprite.cpp
    src/rendering/ui.cpp
    src/configs.cpp
)

target_include_directories(sandbox PRIVATE
        ${SDL2_INCLUDE_DIRS}
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/include/core
        ${CMAKE_SOURCE_DIR}/include/shared
        ${CMAKE_SOURCE_DIR}/include/rendering
        ${CMAKE_SOURCE_DIR}/include/sandbox
)

target_link_libraries(sandbox PRIVATE
    SDL2::SDL2
    SDL2_image::SDL2_image
)

# Set output dir for the shared library
set_target_properties(sandbox PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build
)
