file(GLOB SOURCES_CPP
    "${CMAKE_CURRENT_LIST_DIR}/backends/*.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/*.cpp"
)

file(GLOB SOURCES_H
    "${CMAKE_CURRENT_LIST_DIR}/backends/*.h"
    "${CMAKE_CURRENT_LIST_DIR}/*.h"
)

add_library(imgui STATIC ${SOURCES_CPP} ${SOURCES_H})
target_include_directories(imgui PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})