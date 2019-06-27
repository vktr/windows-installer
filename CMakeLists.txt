cmake_minimum_required(VERSION 3.15)

project(windows-installer)

add_library(
    ${PROJECT_NAME}
    SHARED
    src/database
    src/preview
    src/record
    src/summary-information
    src/view
    src/windows-installer
)

target_include_directories(
    ${PROJECT_NAME}
    PRIVATE
    ${CMAKE_JS_INC}
)

set_target_properties(${PROJECT_NAME} PROPERTIES PREFIX "" SUFFIX ".node")

target_link_libraries(
    ${PROJECT_NAME}
    ${CMAKE_JS_LIB}
    msi
)
