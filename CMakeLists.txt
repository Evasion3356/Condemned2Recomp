# condemned2recomp - ReXGlue Recompiled Project
#
# SDK boilerplate lives in generated/rexglue.cmake.

cmake_minimum_required(VERSION 3.25)
project(condemned2recomp LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Use the bundled SDK submodule unless REXSDK_DIR / an installed SDK is provided.
if(NOT REXSDK_DIR AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/rexglue-sdk/CMakeLists.txt")
    set(REXSDK_DIR "${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/rexglue-sdk" CACHE PATH
        "Path to rexglue-sdk source tree" FORCE)
endif()

# Windows checkouts without symlink support turn libmspack's symlinked sources
# into small text files containing the link target. Replace them with copies.
if(WIN32 AND REXSDK_DIR)
    file(GLOB _mspack_files "${REXSDK_DIR}/thirdparty/libmspack/cabextract/mspack/*")
    foreach(_f IN LISTS _mspack_files)
        if(IS_DIRECTORY "${_f}")
            continue()
        endif()
        file(SIZE "${_f}" _size)
        if(_size LESS 256)
            file(READ "${_f}" _content)
            string(STRIP "${_content}" _target)
            get_filename_component(_dir "${_f}" DIRECTORY)
            if(_target MATCHES "^\\.\\./.*mspack/" AND EXISTS "${_dir}/${_target}")
                message(STATUS "Replacing symlink stub ${_f}")
                file(COPY_FILE "${_dir}/${_target}" "${_f}" ONLY_IF_DIFFERENT)
            endif()
        endif()
    endforeach()
endif()

include(generated/rexglue.cmake)

# -------------------------------------------------
# RexGlue-SDk / Condemned2Recomp Sources
# -------------------------------------------------
set(CONDEMNED2RECOMP_SOURCES
    src/main.cpp
    src/condemned2_settings.cpp
    src/condemned2_input.cpp
    src/condemned2_graphics.cpp
    src/condemned2_hooks.cpp
    src/condemned2_cheats.cpp
    src/condemned2_debug_server.cpp
    src/condemned2_multiplayer.cpp
)
if(WIN32)
    add_executable(condemned2recomp WIN32 ${CONDEMNED2RECOMP_SOURCES})
else()
    add_executable(condemned2recomp ${CONDEMNED2RECOMP_SOURCES})
endif()

# Building the SDK from source doesn't export its bundled ImGui headers.
if(REXSDK_DIR)
    target_include_directories(condemned2recomp PRIVATE "${REXSDK_DIR}/thirdparty/imgui")
endif()

rexglue_setup_target(condemned2recomp GPU_PLUGINS xenos)

if(WIN32)
    target_link_libraries(condemned2recomp PRIVATE ws2_32)
endif()

# -------------------------------------------------
# Condemned2Recomp Updater
# -------------------------------------------------
# This simply replaces the main app with the downloaded update
set(CONDEMNED2RECOMP_UPDATE_SOURCES
    src/main_updater.cpp
)
add_executable(condemned2recomp_updater ${CONDEMNED2RECOMP_UPDATE_SOURCES}) 
target_include_directories(condemned2recomp_updater PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

# -------------------------------------------------
# Dependencies
# -------------------------------------------------
# See THIRD-PARTY-NOTICES
set(_prev_shared ${BUILD_SHARED_LIBS})
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
include(FetchContent)

# -------------------------------------------------
# zlib
# -------------------------------------------------
FetchContent_Declare(zlib
  GIT_REPOSITORY https://github.com/madler/zlib.git
  GIT_TAG v1.3.1
  GIT_SHALLOW TRUE
)
set(LIBZIP_DO_INSTALL OFF CACHE BOOL "Disable libzip install/export rules when embedded" FORCE)
set(ENABLE_BZIP2 OFF CACHE BOOL "" FORCE)
set(ENABLE_LZMA OFF CACHE BOOL "" FORCE)
set(ENABLE_ZSTD OFF CACHE BOOL "" FORCE)
set(BUILD_TOOLS OFF CACHE BOOL "" FORCE)
set(BUILD_REGRESS OFF CACHE BOOL "" FORCE)
set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(BUILD_DOC OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(zlib)
# libzip's find_package(ZLIB) can't see the FetchContent build, so point it at it.
set(ZLIB_INCLUDE_DIR "${zlib_SOURCE_DIR}" "${zlib_BINARY_DIR}" CACHE STRING "" FORCE)
if(TARGET zlibstatic)
  set(ZLIB_LIBRARY zlibstatic CACHE STRING "" FORCE)
else()
  set(ZLIB_LIBRARY zlib CACHE STRING "" FORCE)
endif()
if (TARGET zlibstatic AND NOT TARGET ZLIB::ZLIB)
  add_library(ZLIB::ZLIB ALIAS zlibstatic)
elseif (TARGET zlib AND NOT TARGET ZLIB::ZLIB)
  add_library(ZLIB::ZLIB ALIAS zlib)
endif()

# -------------------------------------------------
# curl
# -------------------------------------------------
FetchContent_Declare(curl
  GIT_REPOSITORY https://github.com/curl/curl.git
  GIT_TAG curl-8_10_1
  GIT_SHALLOW TRUE
)
set(HTTP_ONLY ON CACHE BOOL "" FORCE)
set(BUILD_CURL_EXE OFF CACHE BOOL "" FORCE)
set(BUILD_TESTING OFF CACHE BOOL "" FORCE)
set(CURL_USE_LIBPSL OFF CACHE BOOL "" FORCE)
set(CURL_ZLIB OFF CACHE BOOL "" FORCE)
set(CURL_DISABLE_LDAP ON CACHE BOOL "" FORCE)
set(CURL_DISABLE_INSTALL ON CACHE BOOL "" FORCE)
if(WIN32)
  set(CURL_USE_SCHANNEL ON CACHE BOOL "" FORCE)
else()
  set(CURL_USE_OPENSSL ON CACHE BOOL "" FORCE)
endif()
FetchContent_MakeAvailable(curl)
target_link_libraries(condemned2recomp PRIVATE CURL::libcurl )

# -------------------------------------------------
# libzip
# -------------------------------------------------
FetchContent_Declare(libzip
  GIT_REPOSITORY https://github.com/nih-at/libzip.git
  GIT_TAG 8e037534ccd62f444abbe4ce265443ee74bbfeb3 #v1.10.1
  GIT_SHALLOW TRUE
)
set(ENABLE_BZIP2 OFF CACHE BOOL "" FORCE)
set(ENABLE_LZMA OFF CACHE BOOL "" FORCE)
set(ENABLE_ZSTD OFF CACHE BOOL "" FORCE)
set(BUILD_TOOLS OFF CACHE BOOL "" FORCE)
set(BUILD_REGRESS OFF CACHE BOOL "" FORCE)
set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(BUILD_DOC OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(libzip)
target_link_libraries(condemned2recomp PRIVATE libzip::zip)
target_link_libraries(condemned2recomp_updater PRIVATE libzip::zip)

# -------------------------------------------------
# PicoSHA2
# -------------------------------------------------
FetchContent_Declare(pico
    GIT_REPOSITORY https://github.com/okdshin/PicoSHA2
    GIT_TAG 161cb3fc4170fa7a3eca9e582cebd27cc4d1fe29
    GIT_SHALLOW TRUE)
FetchContent_MakeAvailable(pico)
target_include_directories(condemned2recomp PRIVATE ${pico_SOURCE_DIR} )
target_include_directories(condemned2recomp_updater PRIVATE ${pico_SOURCE_DIR} )
set(BUILD_SHARED_LIBS ${_prev_shared} CACHE BOOL "" FORCE)

# -------------------------------------------------
# json
# -------------------------------------------------
include(FetchContent)
FetchContent_Declare(json
  GIT_REPOSITORY https://github.com/nlohmann/json.git
  GIT_TAG 55f93686c01528224f448c19128836e7df245f72
)
FetchContent_MakeAvailable(json)
target_link_libraries(condemned2recomp PRIVATE nlohmann_json::nlohmann_json)
