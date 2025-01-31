
cmake_minimum_required(VERSION 3.10)
project(server)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# For Debugging
set(CMAKE_BUILD_TYPE Debug)  # Forces debug mode
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0")

include_directories(
    ${CMAKE_SOURCE_DIR}/include
)

set(SOURCES
    src/main.cpp
    src/udp_server.cpp
    src/packet.cpp
    src/player.cpp
)

add_executable(server
    ${SOURCES}
)

