# Install script for directory: /Users/aryanrogye/Documents/CODE/BigProjects/GameEngineBase/SDL2

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/usr/local")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Release")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "TRUE")
endif()

# Set path to fallback-tool for dependency-resolution.
if(NOT DEFINED CMAKE_OBJDUMP)
  set(CMAKE_OBJDUMP "/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/objdump")
endif()

set(CMAKE_BINARY_DIR "/Users/aryanrogye/Documents/CODE/BigProjects/GameEngineBase/GameEngine/build_ios")

if(NOT PLATFORM_NAME)
  if(NOT "$ENV{PLATFORM_NAME}" STREQUAL "")
    set(PLATFORM_NAME "$ENV{PLATFORM_NAME}")
  endif()
  if(NOT PLATFORM_NAME)
    set(PLATFORM_NAME iphoneos)
  endif()
endif()

if(NOT EFFECTIVE_PLATFORM_NAME)
  if(NOT "$ENV{EFFECTIVE_PLATFORM_NAME}" STREQUAL "")
    set(EFFECTIVE_PLATFORM_NAME "$ENV{EFFECTIVE_PLATFORM_NAME}")
  endif()
  if(NOT EFFECTIVE_PLATFORM_NAME)
    set(EFFECTIVE_PLATFORM_NAME -iphoneos)
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE SHARED_LIBRARY FILES "/Users/aryanrogye/Documents/CODE/BigProjects/GameEngineBase/GameEngine/build_ios/SDL2/Debug${EFFECTIVE_PLATFORM_NAME}/libSDL2-2.0d.dylib")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE SHARED_LIBRARY FILES "/Users/aryanrogye/Documents/CODE/BigProjects/GameEngineBase/GameEngine/build_ios/SDL2/Release${EFFECTIVE_PLATFORM_NAME}/libSDL2-2.0.dylib")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Mm][Ii][Nn][Ss][Ii][Zz][Ee][Rr][Ee][Ll])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE SHARED_LIBRARY FILES "/Users/aryanrogye/Documents/CODE/BigProjects/GameEngineBase/GameEngine/build_ios/SDL2/MinSizeRel${EFFECTIVE_PLATFORM_NAME}/libSDL2-2.0.dylib")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE SHARED_LIBRARY FILES "/Users/aryanrogye/Documents/CODE/BigProjects/GameEngineBase/GameEngine/build_ios/SDL2/RelWithDebInfo${EFFECTIVE_PLATFORM_NAME}/libSDL2-2.0.dylib")
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Mm][Ii][Nn][Ss][Ii][Zz][Ee][Rr][Ee][Ll])$")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "/Users/aryanrogye/Documents/CODE/BigProjects/GameEngineBase/GameEngine/build_ios/SDL2/Debug${EFFECTIVE_PLATFORM_NAME}/libSDL2maind.a")
    if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libSDL2maind.a" AND
       NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libSDL2maind.a")
      execute_process(COMMAND "/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/ranlib" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libSDL2maind.a")
    endif()
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "/Users/aryanrogye/Documents/CODE/BigProjects/GameEngineBase/GameEngine/build_ios/SDL2/Release${EFFECTIVE_PLATFORM_NAME}/libSDL2main.a")
    if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libSDL2main.a" AND
       NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libSDL2main.a")
      execute_process(COMMAND "/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/ranlib" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libSDL2main.a")
    endif()
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Mm][Ii][Nn][Ss][Ii][Zz][Ee][Rr][Ee][Ll])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "/Users/aryanrogye/Documents/CODE/BigProjects/GameEngineBase/GameEngine/build_ios/SDL2/MinSizeRel${EFFECTIVE_PLATFORM_NAME}/libSDL2main.a")
    if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libSDL2main.a" AND
       NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libSDL2main.a")
      execute_process(COMMAND "/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/ranlib" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libSDL2main.a")
    endif()
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "/Users/aryanrogye/Documents/CODE/BigProjects/GameEngineBase/GameEngine/build_ios/SDL2/RelWithDebInfo${EFFECTIVE_PLATFORM_NAME}/libSDL2main.a")
    if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libSDL2main.a" AND
       NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libSDL2main.a")
      execute_process(COMMAND "/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/ranlib" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libSDL2main.a")
    endif()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "/Users/aryanrogye/Documents/CODE/BigProjects/GameEngineBase/GameEngine/build_ios/SDL2/Debug${EFFECTIVE_PLATFORM_NAME}/libSDL2d.a")
    if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libSDL2d.a" AND
       NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libSDL2d.a")
      execute_process(COMMAND "/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/ranlib" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libSDL2d.a")
    endif()
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "/Users/aryanrogye/Documents/CODE/BigProjects/GameEngineBase/GameEngine/build_ios/SDL2/Release${EFFECTIVE_PLATFORM_NAME}/libSDL2.a")
    if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libSDL2.a" AND
       NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libSDL2.a")
      execute_process(COMMAND "/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/ranlib" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libSDL2.a")
    endif()
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Mm][Ii][Nn][Ss][Ii][Zz][Ee][Rr][Ee][Ll])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "/Users/aryanrogye/Documents/CODE/BigProjects/GameEngineBase/GameEngine/build_ios/SDL2/MinSizeRel${EFFECTIVE_PLATFORM_NAME}/libSDL2.a")
    if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libSDL2.a" AND
       NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libSDL2.a")
      execute_process(COMMAND "/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/ranlib" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libSDL2.a")
    endif()
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "/Users/aryanrogye/Documents/CODE/BigProjects/GameEngineBase/GameEngine/build_ios/SDL2/RelWithDebInfo${EFFECTIVE_PLATFORM_NAME}/libSDL2.a")
    if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libSDL2.a" AND
       NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libSDL2.a")
      execute_process(COMMAND "/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/ranlib" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libSDL2.a")
    endif()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/SDL2/SDL2Targets.cmake")
    file(DIFFERENT _cmake_export_file_changed FILES
         "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/SDL2/SDL2Targets.cmake"
         "/Users/aryanrogye/Documents/CODE/BigProjects/GameEngineBase/GameEngine/build_ios/SDL2/CMakeFiles/Export/f084604df1a27ef5b4fef7c7544737d1/SDL2Targets.cmake")
    if(_cmake_export_file_changed)
      file(GLOB _cmake_old_config_files "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/SDL2/SDL2Targets-*.cmake")
      if(_cmake_old_config_files)
        string(REPLACE ";" ", " _cmake_old_config_files_text "${_cmake_old_config_files}")
        message(STATUS "Old export file \"$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/SDL2/SDL2Targets.cmake\" will be replaced.  Removing files [${_cmake_old_config_files_text}].")
        unset(_cmake_old_config_files_text)
        file(REMOVE ${_cmake_old_config_files})
      endif()
      unset(_cmake_old_config_files)
    endif()
    unset(_cmake_export_file_changed)
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/SDL2" TYPE FILE FILES "/Users/aryanrogye/Documents/CODE/BigProjects/GameEngineBase/GameEngine/build_ios/SDL2/CMakeFiles/Export/f084604df1a27ef5b4fef7c7544737d1/SDL2Targets.cmake")
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/SDL2" TYPE FILE FILES "/Users/aryanrogye/Documents/CODE/BigProjects/GameEngineBase/GameEngine/build_ios/SDL2/CMakeFiles/Export/f084604df1a27ef5b4fef7c7544737d1/SDL2Targets-debug.cmake")
  endif()
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Mm][Ii][Nn][Ss][Ii][Zz][Ee][Rr][Ee][Ll])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/SDL2" TYPE FILE FILES "/Users/aryanrogye/Documents/CODE/BigProjects/GameEngineBase/GameEngine/build_ios/SDL2/CMakeFiles/Export/f084604df1a27ef5b4fef7c7544737d1/SDL2Targets-minsizerel.cmake")
  endif()
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/SDL2" TYPE FILE FILES "/Users/aryanrogye/Documents/CODE/BigProjects/GameEngineBase/GameEngine/build_ios/SDL2/CMakeFiles/Export/f084604df1a27ef5b4fef7c7544737d1/SDL2Targets-relwithdebinfo.cmake")
  endif()
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/SDL2" TYPE FILE FILES "/Users/aryanrogye/Documents/CODE/BigProjects/GameEngineBase/GameEngine/build_ios/SDL2/CMakeFiles/Export/f084604df1a27ef5b4fef7c7544737d1/SDL2Targets-release.cmake")
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/SDL2/SDL2mainTargets.cmake")
    file(DIFFERENT _cmake_export_file_changed FILES
         "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/SDL2/SDL2mainTargets.cmake"
         "/Users/aryanrogye/Documents/CODE/BigProjects/GameEngineBase/GameEngine/build_ios/SDL2/CMakeFiles/Export/f084604df1a27ef5b4fef7c7544737d1/SDL2mainTargets.cmake")
    if(_cmake_export_file_changed)
      file(GLOB _cmake_old_config_files "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/SDL2/SDL2mainTargets-*.cmake")
      if(_cmake_old_config_files)
        string(REPLACE ";" ", " _cmake_old_config_files_text "${_cmake_old_config_files}")
        message(STATUS "Old export file \"$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/SDL2/SDL2mainTargets.cmake\" will be replaced.  Removing files [${_cmake_old_config_files_text}].")
        unset(_cmake_old_config_files_text)
        file(REMOVE ${_cmake_old_config_files})
      endif()
      unset(_cmake_old_config_files)
    endif()
    unset(_cmake_export_file_changed)
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/SDL2" TYPE FILE FILES "/Users/aryanrogye/Documents/CODE/BigProjects/GameEngineBase/GameEngine/build_ios/SDL2/CMakeFiles/Export/f084604df1a27ef5b4fef7c7544737d1/SDL2mainTargets.cmake")
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/SDL2" TYPE FILE FILES "/Users/aryanrogye/Documents/CODE/BigProjects/GameEngineBase/GameEngine/build_ios/SDL2/CMakeFiles/Export/f084604df1a27ef5b4fef7c7544737d1/SDL2mainTargets-debug.cmake")
  endif()
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Mm][Ii][Nn][Ss][Ii][Zz][Ee][Rr][Ee][Ll])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/SDL2" TYPE FILE FILES "/Users/aryanrogye/Documents/CODE/BigProjects/GameEngineBase/GameEngine/build_ios/SDL2/CMakeFiles/Export/f084604df1a27ef5b4fef7c7544737d1/SDL2mainTargets-minsizerel.cmake")
  endif()
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/SDL2" TYPE FILE FILES "/Users/aryanrogye/Documents/CODE/BigProjects/GameEngineBase/GameEngine/build_ios/SDL2/CMakeFiles/Export/f084604df1a27ef5b4fef7c7544737d1/SDL2mainTargets-relwithdebinfo.cmake")
  endif()
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/SDL2" TYPE FILE FILES "/Users/aryanrogye/Documents/CODE/BigProjects/GameEngineBase/GameEngine/build_ios/SDL2/CMakeFiles/Export/f084604df1a27ef5b4fef7c7544737d1/SDL2mainTargets-release.cmake")
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/SDL2/SDL2staticTargets.cmake")
    file(DIFFERENT _cmake_export_file_changed FILES
         "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/SDL2/SDL2staticTargets.cmake"
         "/Users/aryanrogye/Documents/CODE/BigProjects/GameEngineBase/GameEngine/build_ios/SDL2/CMakeFiles/Export/f084604df1a27ef5b4fef7c7544737d1/SDL2staticTargets.cmake")
    if(_cmake_export_file_changed)
      file(GLOB _cmake_old_config_files "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/SDL2/SDL2staticTargets-*.cmake")
      if(_cmake_old_config_files)
        string(REPLACE ";" ", " _cmake_old_config_files_text "${_cmake_old_config_files}")
        message(STATUS "Old export file \"$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/SDL2/SDL2staticTargets.cmake\" will be replaced.  Removing files [${_cmake_old_config_files_text}].")
        unset(_cmake_old_config_files_text)
        file(REMOVE ${_cmake_old_config_files})
      endif()
      unset(_cmake_old_config_files)
    endif()
    unset(_cmake_export_file_changed)
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/SDL2" TYPE FILE FILES "/Users/aryanrogye/Documents/CODE/BigProjects/GameEngineBase/GameEngine/build_ios/SDL2/CMakeFiles/Export/f084604df1a27ef5b4fef7c7544737d1/SDL2staticTargets.cmake")
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/SDL2" TYPE FILE FILES "/Users/aryanrogye/Documents/CODE/BigProjects/GameEngineBase/GameEngine/build_ios/SDL2/CMakeFiles/Export/f084604df1a27ef5b4fef7c7544737d1/SDL2staticTargets-debug.cmake")
  endif()
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Mm][Ii][Nn][Ss][Ii][Zz][Ee][Rr][Ee][Ll])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/SDL2" TYPE FILE FILES "/Users/aryanrogye/Documents/CODE/BigProjects/GameEngineBase/GameEngine/build_ios/SDL2/CMakeFiles/Export/f084604df1a27ef5b4fef7c7544737d1/SDL2staticTargets-minsizerel.cmake")
  endif()
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/SDL2" TYPE FILE FILES "/Users/aryanrogye/Documents/CODE/BigProjects/GameEngineBase/GameEngine/build_ios/SDL2/CMakeFiles/Export/f084604df1a27ef5b4fef7c7544737d1/SDL2staticTargets-relwithdebinfo.cmake")
  endif()
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/SDL2" TYPE FILE FILES "/Users/aryanrogye/Documents/CODE/BigProjects/GameEngineBase/GameEngine/build_ios/SDL2/CMakeFiles/Export/f084604df1a27ef5b4fef7c7544737d1/SDL2staticTargets-release.cmake")
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Devel" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/SDL2" TYPE FILE FILES
    "/Users/aryanrogye/Documents/CODE/BigProjects/GameEngineBase/SDL2/SDL2Config.cmake"
    "/Users/aryanrogye/Documents/CODE/BigProjects/GameEngineBase/GameEngine/build_ios/SDL2ConfigVersion.cmake"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/SDL2" TYPE FILE FILES
    "/Users/aryanrogye/Documents/CODE/BigProjects/GameEngineBase/SDL2/include/SDL.h"
    "/Users/aryanrogye/Documents/CODE/BigProjects/GameEngineBase/SDL2/include/SDL_assert.h"
    "/Users/aryanrogye/Documents/CODE/BigProjects/GameEngineBase/SDL2/include/SDL_atomic.h"
    "/Users/aryanrogye/Documents/CODE/BigProjects/GameEngineBase/SDL2/include/SDL_audio.h"
    "/Users/aryanrogye/Documents/CODE/BigProjects/GameEngineBase/SDL2/include/SDL_bits.h"
    "/Users/aryanrogye/Documents/CODE/BigProjects/GameEngineBase/SDL2/include/SDL_blendmode.h"
    "/Users/aryanrogye/Documents/CODE/BigProjects/GameEngineBase/SDL2/include/SDL_clipboard.h"
    "/Users/aryanrogye/Documents/CODE/BigProjects/GameEngineBase/SDL2/include/SDL_config_android.h"
    "/Users/aryanrogye/Documents/CODE/BigProjects/GameEngineBase/SDL2/include/SDL_config_emscripten.h"
    "/Users/aryanrogye/Documents/CODE/BigProjects/GameEngineBase/SDL2/include/SDL_config_iphoneos.h"
    "/Users/aryanrogye/Documents/CODE/BigProjects/GameEngineBase/SDL2/include/SDL_config_macosx.h"
    "/Users/aryanrogye/Documents/CODE/BigProjects/GameEngineBase/SDL2/include/SDL_config_minimal.h"
    "/Users/aryanrogye/Documents/CODE/BigProjects/GameEngineBase/SDL2/include/SDL_config_os2.h"
    "/Users/aryanrogye/Documents/CODE/BigProjects/GameEngineBase/SDL2/include/SDL_config_pandora.h"
    "/Users/aryanrogye/Documents/CODE/BigProjects/GameEngineBase/SDL2/include/SDL_config_windows.h"
    "/Users/aryanrogye/Documents/CODE/BigProjects/GameEngineBase/SDL2/include/SDL_config_winrt.h"
    "/Users/aryanrogye/Documents/CODE/BigProjects/GameEngineBase/SDL2/include/SDL_copying.h"
    "/Users/aryanrogye/Documents/CODE/BigProjects/GameEngineBase/SDL2/include/SDL_cpuinfo.h"
    "/Users/aryanrogye/Documents/CODE/BigProjects/GameEngineBase/SDL2/include/SDL_egl.h"
    "/Users/aryanrogye/Documents/CODE/BigProjects/GameEngineBase/SDL2/include/SDL_endian.h"
    "/Users/aryanrogye/Documents/CODE/BigProjects/GameEngineBase/SDL2/include/SDL_error.h"
    "/Users/aryanrogye/Documents/CODE/BigProjects/GameEngineBase/SDL2/include/SDL_events.h"
    "/Users/aryanrogye/Documents/CODE/BigProjects/GameEngineBase/SDL2/include/SDL_filesystem.h"
    "/Users/aryanrogye/Documents/CODE/BigProjects/GameEngineBase/SDL2/include/SDL_gamecontroller.h"
    "/Users/aryanrogye/Documents/CODE/BigProjects/GameEngineBase/SDL2/include/SDL_gesture.h"
    "/Users/aryanrogye/Documents/CODE/BigProjects/GameEngineBase/SDL2/include/SDL_haptic.h"
    "/Users/aryanrogye/Documents/CODE/BigProjects/GameEngineBase/SDL2/include/SDL_hidapi.h"
    "/Users/aryanrogye/Documents/CODE/BigProjects/GameEngineBase/SDL2/include/SDL_hints.h"
    "/Users/aryanrogye/Documents/CODE/BigProjects/GameEngineBase/SDL2/include/SDL_joystick.h"
    "/Users/aryanrogye/Documents/CODE/BigProjects/GameEngineBase/SDL2/include/SDL_keyboard.h"
    "/Users/aryanrogye/Documents/CODE/BigProjects/GameEngineBase/SDL2/include/SDL_keycode.h"
    "/Users/aryanrogye/Documents/CODE/BigProjects/GameEngineBase/SDL2/include/SDL_loadso.h"
    "/Users/aryanrogye/Documents/CODE/BigProjects/GameEngineBase/SDL2/include/SDL_locale.h"
    "/Users/aryanrogye/Documents/CODE/BigProjects/GameEngineBase/SDL2/include/SDL_log.h"
    "/Users/aryanrogye/Documents/CODE/BigProjects/GameEngineBase/SDL2/include/SDL_main.h"
    "/Users/aryanrogye/Documents/CODE/BigProjects/GameEngineBase/SDL2/include/SDL_messagebox.h"
    "/Users/aryanrogye/Documents/CODE/BigProjects/GameEngineBase/SDL2/include/SDL_metal.h"
    "/Users/aryanrogye/Documents/CODE/BigProjects/GameEngineBase/SDL2/include/SDL_misc.h"
    "/Users/aryanrogye/Documents/CODE/BigProjects/GameEngineBase/SDL2/include/SDL_mouse.h"
    "/Users/aryanrogye/Documents/CODE/BigProjects/GameEngineBase/SDL2/include/SDL_mutex.h"
    "/Users/aryanrogye/Documents/CODE/BigProjects/GameEngineBase/SDL2/include/SDL_name.h"
    "/Users/aryanrogye/Documents/CODE/BigProjects/GameEngineBase/SDL2/include/SDL_opengl.h"
    "/Users/aryanrogye/Documents/CODE/BigProjects/GameEngineBase/SDL2/include/SDL_opengl_glext.h"
    "/Users/aryanrogye/Documents/CODE/BigProjects/GameEngineBase/SDL2/include/SDL_opengles.h"
    "/Users/aryanrogye/Documents/CODE/BigProjects/GameEngineBase/SDL2/include/SDL_opengles2.h"
    "/Users/aryanrogye/Documents/CODE/BigProjects/GameEngineBase/SDL2/include/SDL_opengles2_gl2.h"
    "/Users/aryanrogye/Documents/CODE/BigProjects/GameEngineBase/SDL2/include/SDL_opengles2_gl2ext.h"
    "/Users/aryanrogye/Documents/CODE/BigProjects/GameEngineBase/SDL2/include/SDL_opengles2_gl2platform.h"
    "/Users/aryanrogye/Documents/CODE/BigProjects/GameEngineBase/SDL2/include/SDL_opengles2_khrplatform.h"
    "/Users/aryanrogye/Documents/CODE/BigProjects/GameEngineBase/SDL2/include/SDL_pixels.h"
    "/Users/aryanrogye/Documents/CODE/BigProjects/GameEngineBase/SDL2/include/SDL_platform.h"
    "/Users/aryanrogye/Documents/CODE/BigProjects/GameEngineBase/SDL2/include/SDL_power.h"
    "/Users/aryanrogye/Documents/CODE/BigProjects/GameEngineBase/SDL2/include/SDL_quit.h"
    "/Users/aryanrogye/Documents/CODE/BigProjects/GameEngineBase/SDL2/include/SDL_rect.h"
    "/Users/aryanrogye/Documents/CODE/BigProjects/GameEngineBase/SDL2/include/SDL_render.h"
    "/Users/aryanrogye/Documents/CODE/BigProjects/GameEngineBase/SDL2/include/SDL_rwops.h"
    "/Users/aryanrogye/Documents/CODE/BigProjects/GameEngineBase/SDL2/include/SDL_scancode.h"
    "/Users/aryanrogye/Documents/CODE/BigProjects/GameEngineBase/SDL2/include/SDL_sensor.h"
    "/Users/aryanrogye/Documents/CODE/BigProjects/GameEngineBase/SDL2/include/SDL_shape.h"
    "/Users/aryanrogye/Documents/CODE/BigProjects/GameEngineBase/SDL2/include/SDL_stdinc.h"
    "/Users/aryanrogye/Documents/CODE/BigProjects/GameEngineBase/SDL2/include/SDL_surface.h"
    "/Users/aryanrogye/Documents/CODE/BigProjects/GameEngineBase/SDL2/include/SDL_system.h"
    "/Users/aryanrogye/Documents/CODE/BigProjects/GameEngineBase/SDL2/include/SDL_syswm.h"
    "/Users/aryanrogye/Documents/CODE/BigProjects/GameEngineBase/SDL2/include/SDL_test.h"
    "/Users/aryanrogye/Documents/CODE/BigProjects/GameEngineBase/SDL2/include/SDL_test_assert.h"
    "/Users/aryanrogye/Documents/CODE/BigProjects/GameEngineBase/SDL2/include/SDL_test_common.h"
    "/Users/aryanrogye/Documents/CODE/BigProjects/GameEngineBase/SDL2/include/SDL_test_compare.h"
    "/Users/aryanrogye/Documents/CODE/BigProjects/GameEngineBase/SDL2/include/SDL_test_crc32.h"
    "/Users/aryanrogye/Documents/CODE/BigProjects/GameEngineBase/SDL2/include/SDL_test_font.h"
    "/Users/aryanrogye/Documents/CODE/BigProjects/GameEngineBase/SDL2/include/SDL_test_fuzzer.h"
    "/Users/aryanrogye/Documents/CODE/BigProjects/GameEngineBase/SDL2/include/SDL_test_harness.h"
    "/Users/aryanrogye/Documents/CODE/BigProjects/GameEngineBase/SDL2/include/SDL_test_images.h"
    "/Users/aryanrogye/Documents/CODE/BigProjects/GameEngineBase/SDL2/include/SDL_test_log.h"
    "/Users/aryanrogye/Documents/CODE/BigProjects/GameEngineBase/SDL2/include/SDL_test_md5.h"
    "/Users/aryanrogye/Documents/CODE/BigProjects/GameEngineBase/SDL2/include/SDL_test_memory.h"
    "/Users/aryanrogye/Documents/CODE/BigProjects/GameEngineBase/SDL2/include/SDL_test_random.h"
    "/Users/aryanrogye/Documents/CODE/BigProjects/GameEngineBase/SDL2/include/SDL_thread.h"
    "/Users/aryanrogye/Documents/CODE/BigProjects/GameEngineBase/SDL2/include/SDL_timer.h"
    "/Users/aryanrogye/Documents/CODE/BigProjects/GameEngineBase/SDL2/include/SDL_touch.h"
    "/Users/aryanrogye/Documents/CODE/BigProjects/GameEngineBase/SDL2/include/SDL_types.h"
    "/Users/aryanrogye/Documents/CODE/BigProjects/GameEngineBase/SDL2/include/SDL_version.h"
    "/Users/aryanrogye/Documents/CODE/BigProjects/GameEngineBase/SDL2/include/SDL_video.h"
    "/Users/aryanrogye/Documents/CODE/BigProjects/GameEngineBase/SDL2/include/SDL_vulkan.h"
    "/Users/aryanrogye/Documents/CODE/BigProjects/GameEngineBase/SDL2/include/begin_code.h"
    "/Users/aryanrogye/Documents/CODE/BigProjects/GameEngineBase/SDL2/include/close_code.h"
    "/Users/aryanrogye/Documents/CODE/BigProjects/GameEngineBase/GameEngine/build_ios/SDL2/include/SDL_config.h"
    "/Users/aryanrogye/Documents/CODE/BigProjects/GameEngineBase/GameEngine/build_ios/SDL2/include/SDL_revision.h"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  
            execute_process(COMMAND /opt/homebrew/Cellar/cmake/3.30.4/bin/cmake -E create_symlink
              "libSDL2-2.0d.dylib" "libSDL2d.dylib"
              WORKING_DIRECTORY "/Users/aryanrogye/Documents/CODE/BigProjects/GameEngineBase/GameEngine/build_ios/SDL2")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE FILE FILES "/Users/aryanrogye/Documents/CODE/BigProjects/GameEngineBase/GameEngine/build_ios/SDL2/libSDL2d.dylib")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/pkgconfig" TYPE FILE FILES "/Users/aryanrogye/Documents/CODE/BigProjects/GameEngineBase/GameEngine/build_ios/SDL2/sdl2.pc")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE PROGRAM FILES "/Users/aryanrogye/Documents/CODE/BigProjects/GameEngineBase/GameEngine/build_ios/SDL2/sdl2-config")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/aclocal" TYPE FILE FILES "/Users/aryanrogye/Documents/CODE/BigProjects/GameEngineBase/SDL2/sdl2.m4")
endif()

