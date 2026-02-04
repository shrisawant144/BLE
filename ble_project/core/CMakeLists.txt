cmake_minimum_required(VERSION 3.5)

add_library(ble_core STATIC
    src/ble_common.c
)

target_include_directories(ble_core PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)
