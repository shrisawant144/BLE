cmake_minimum_required(VERSION 3.5)

find_library(GATTLIB gattlib REQUIRED)

set(EXAMPLES
    01_scan
    02_connect
    03_read_write
    04_notifications
    05_heart_rate
    06_nordic_uart
)

foreach(EXAMPLE ${EXAMPLES})
    file(GLOB SOURCES ${EXAMPLE}/*.cpp)
    if(SOURCES)
        get_filename_component(EXEC_NAME ${SOURCES} NAME_WE)
        add_executable(${EXEC_NAME} ${SOURCES})
        target_link_libraries(${EXEC_NAME} ${GATTLIB} ble_core)
        set_target_properties(${EXEC_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
    endif()
endforeach()
