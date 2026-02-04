cmake_minimum_required(VERSION 3.5)

pkg_check_modules(GLIB REQUIRED glib-2.0)
pkg_check_modules(GIO REQUIRED gio-2.0)

set(EXAMPLES
    01_simple_peripheral
    02_notify
)

foreach(EXAMPLE ${EXAMPLES})
    file(GLOB SOURCES ${EXAMPLE}/*.cpp)
    if(SOURCES)
        get_filename_component(EXEC_NAME ${SOURCES} NAME_WE)
        add_executable(${EXEC_NAME} ${SOURCES})
        target_include_directories(${EXEC_NAME} PRIVATE ${GLIB_INCLUDE_DIRS} ${GIO_INCLUDE_DIRS})
        target_link_libraries(${EXEC_NAME} ${GLIB_LIBRARIES} ${GIO_LIBRARIES} ble_core)
        set_target_properties(${EXEC_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
    endif()
endforeach()
