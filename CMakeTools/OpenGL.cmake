
add_definitions(-DGLFW_INCLUDE_NONL)

set(GLFW_DIR "${CMAKE_SOURCE_DIR}/external/glfw") 
list(APPEND INCLUDE_DIR ${GLAD_DIR}/include)
# Set this to point to an up-to-date GLFW repo
option(GLFW_BUILD_EXAMPLES "Build the GLFW example programs" OFF)
option(GLFW_BUILD_TESTS "Build the GLFW test programs" OFF)
option(GLFW_BUILD_DOCS "Build the GLFW documentation" OFF)
option(GLFW_INSTALL "Generate installation target" OFF)
option(GLFW_DOCUMENT_INTERNALS "Include internals in documentation" OFF)
add_subdirectory(${GLFW_DIR} glfw_binary EXCLUDE_FROM_ALL)
list(APPEND INCLUDE_DIR ${GLFW_DIR}/include)
list(APPEND INCLUDE_DIR ${GLFW_DIR}/deps)