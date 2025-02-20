cmake_minimum_required(VERSION 3.10)
project(prototype_map_system)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)


set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_FLAGS_DEBUG "-g")  # Ensure debug symbols are added

# SDL 2
set(SDL2_DIR "/opt/homebrew/opt/sdl2/lib/cmake/SDL2")
find_package(SDL2 REQUIRED)
# SDL 2 Image
set(SDL2_IMAGE_DIR "/opt/homebrew/opt/sdl2_image/lib/cmake/SDL2_image")
find_package(SDL2_image REQUIRED)
# SDL 2 TTF
find_package(SDL2_ttf REQUIRED)
# Json
find_package(nlohmann_json REQUIRED)

include_directories("/opt/homebrew/include")
link_directories("/opt/homebrew/lib")

# ImGui Paths
set(IMGUI_DIR 
    ${CMAKE_SOURCE_DIR}/include/third_party/imgui
)
set(
    IMGUI_BACKENDS ${IMGUI_DIR}/backends
)
set(IMGUI_SOURCES
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
    ${IMGUI_BACKENDS}/imgui_impl_sdl2.cpp
    ${IMGUI_BACKENDS}/imgui_impl_sdlrenderer2.cpp
)

add_executable(prototype_map_system

    src/utils/collision.cpp
    src/utils/camera.cpp
    src/utils/sprite.cpp

    src/game.cpp
    src/comfy_lib.cpp
    src/debug_gui.cpp
    src/TSDL.cpp
    src/player.cpp
    src/main.cpp
    ${IMGUI_SOURCES}  # Add ImGui source files to the build
)

target_include_directories(prototype_map_system PRIVATE
        ${SDL2_INCLUDE_DIRS}
        ${SDL2_IMAGE_INCLUDE_DIRS}
        ${SDL2_TTF_INCLUDE_DIRS}
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/include/third_party
        ${IMGUI_DIR}
        ${IMGUI_BACKENDS}
        ${PugiXML_INCLUDE_DIRS}
)

target_link_libraries(prototype_map_system PRIVATE
    SDL2::SDL2
    SDL2_image::SDL2_image
    SDL2_ttf::SDL2_ttf
    nlohmann_json::nlohmann_json
    pugixml
)


