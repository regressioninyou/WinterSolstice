
add_executable(
    Editor 
    "${CMAKE_CURRENT_LIST_DIR}/KyBaoEditor.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/EditorLayout.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Panels/SceneHierarchyPanel.cpp"
)
target_compile_definitions(Editor
PRIVATE
    -DGLFW_INCLUDE_NONL
)
target_sources(
    Editor PUBLIC
        "${CMAKE_CURRENT_LIST_DIR}/EditorLayout.h"
        "${CMAKE_CURRENT_LIST_DIR}/Panels/SceneHierarchyPanel.h"
)
#target_sources(Editor 
#PUBLIC
# "Editor2D.h" 
#)
target_link_libraries(Editor
PRIVATE 
	WinterSolstice-Core
)
set(RESOURCES_DIR ${CMAKE_CURRENT_SOURCE_DIR}/assets)
file(GLOB RESOURCE_FILES ${RESOURCES_DIR}/*)
foreach(RESOURCE ${RESOURCE_FILES})
    file(COPY ${RESOURCE} DESTINATION ${CMAKE_BINARY_DIR}/bin/assets)
endforeach()
target_include_directories(Editor PRIVATE ${CMAKE_BINARY_DIR}/bin/assets)
target_sources(Editor PRIVATE ${RESOURCE_FILES})

find_package(pugixml CONFIG REQUIRED)
target_link_libraries(Editor PRIVATE pugixml::pugixml)
find_package(ASSIMP CONFIG REQUIRED)
target_link_libraries(Editor PRIVATE assimp::assimp)
find_package(spdlog CONFIG REQUIRED)
 target_link_libraries(Editor PRIVATE spdlog::spdlog)
set_target_properties(Editor PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)