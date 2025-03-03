cmake_minimum_required(VERSION 3.10)
project(ComfyGameEngine)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)


set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_FLAGS_DEBUG "-g")  # Ensure debug symbols are added
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fno-omit-frame-pointer")

find_program(ZIG_EXECUTABLE NAMES zig REQUIRED)
set(ZIG_SOURCE 
    ${CMAKE_SOURCE_DIR}/include/comfy_lib.zig
)


# Compile Zig into an object file
add_custom_command(
    OUTPUT comfy_lib_zig.o
    COMMAND ${ZIG_EXECUTABLE} cc -c ${ZIG_SOURCE} -o comfy_lib_zig.o
    DEPENDS ${ZIG_SOURCE}
    COMMENT "Compiling Zig code"
)



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

# Just printing all the paths
message("SDL2_INCLUDE_DIRS: ${SDL2_INCLUDE_DIRS}")
message("SDL2_LIBRARIES: ${SDL2_LIBRARIES}")
message("SDL2_IMAGE_INCLUDE_DIRS: ${SDL2_IMAGE_INCLUDE_DIRS}")
message("SDL2_IMAGE_LIBRARIES: ${SDL2_IMAGE_LIBRARIES}")
message("SDL2_TTF_INCLUDE_DIRS: ${SDL2_TTF_INCLUDE_DIRS}")
message("SDL2_TTF_LIBRARIES: ${SDL2_TTF_LIBRARIES}")

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

add_executable(ComfyGameEngine

    comfy_lib_zig.o

    src/engine/MainView.cpp
    src/engine/GameEngine.cpp
    # UI ELEMENTS OF THE ENGINE
    src/engine/ui/ui_manager.cpp
    src/engine/ui/toolbar.cpp
    src/engine/ui/inspector.cpp
    src/engine/ui/file_explorer.cpp

    src/utils/collision.cpp
    src/utils/camera.cpp
    src/utils/sprite.cpp

    src/entity/player.cpp

    src/gui/debug_gui.cpp
    src/gui/debug_gui_logs.cpp
    src/gui/debug_gui_map.cpp
    src/gui/debug_gui_layers.cpp
    src/gui/debug_gui_textures.cpp
    src/gui/debug_gui_players.cpp
    src/gui/utils/debug_gui_sprites.cpp

    src/game.cpp
    src/comfy_lib.cpp
    src/TSDL.cpp
    src/main.cpp
    ${IMGUI_SOURCES}  # Add ImGui source files to the build
)

target_include_directories(ComfyGameEngine PRIVATE
        ${SDL2_INCLUDE_DIRS}
        ${SDL2_IMAGE_INCLUDE_DIRS}
        ${SDL2_TTF_INCLUDE_DIRS}
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/include/third_party
        ${IMGUI_DIR}
        ${IMGUI_BACKENDS}
        ${PugiXML_INCLUDE_DIRS}
)

target_link_libraries(ComfyGameEngine PRIVATE
    SDL2::SDL2
    SDL2_image::SDL2_image
    SDL2_ttf::SDL2_ttf
    nlohmann_json::nlohmann_json
    pugixml
)


