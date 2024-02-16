add_executable(Testing 
"Test.cpp" 
#"SandBoxAppRender.cpp"
 #"SandBox2D.cpp"
# "TestLoader.h"
)
target_compile_definitions(Testing
PRIVATE
    -DPLATFORM_WINDOWS
    -DGLFW_INCLUDE_NONL
)
#target_sources(SandBox 
#PUBLIC
# "SandBox2D.h"
#)
target_link_libraries(Testing
PRIVATE 
	KyBao
)
set(RESOURCES_DIR ${CMAKE_CURRENT_SOURCE_DIR}/assets)
file(GLOB RESOURCE_FILES ${RESOURCES_DIR}/*)
foreach(RESOURCE ${RESOURCE_FILES})
    file(COPY ${RESOURCE} DESTINATION ${CMAKE_BINARY_DIR}/Testing/assets)
endforeach()
target_include_directories(Testing PRIVATE ${CMAKE_BINARY_DIR}/Testing/assets)
target_sources(Testing PRIVATE ${RESOURCE_FILES})

find_package(pugixml CONFIG REQUIRED)
target_link_libraries(Testing PRIVATE glad) 
target_link_libraries(Testing PRIVATE pugixml::pugixml)
find_package(ASSIMP CONFIG REQUIRED)
target_link_libraries(Testing PRIVATE assimp::assimp)
target_link_libraries(Testing PRIVATE spdlog)
set_target_properties(Testing PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)