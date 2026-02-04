cmake_minimum_required(VERSION 3.5)
project(BLE_Learning_Project VERSION 1.0.0 LANGUAGES C CXX)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

option(BUILD_CENTRAL "Build central examples" ON)
option(BUILD_PERIPHERAL "Build peripheral examples" ON)
option(BUILD_DOCS "Build documentation" OFF)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GLIB REQUIRED glib-2.0)
pkg_check_modules(GIO REQUIRED gio-2.0)

add_subdirectory(core)

if(BUILD_CENTRAL)
    add_subdirectory(central/examples)
endif()

if(BUILD_PERIPHERAL)
    add_subdirectory(peripheral/examples)
endif()

if(BUILD_DOCS AND DOXYGEN_FOUND)
    add_subdirectory(docs)
endif()
