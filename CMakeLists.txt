# CMakeList.txt: WinterSolstice 的 CMake 项目，在此处包括源代码并定义
# 项目特定的逻辑。
#
cmake_minimum_required (VERSION 3.12)

# 如果支持，请为 MSVC 编译器启用热重载。
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("WinterSolstice")

set(CMAKE_PROJECT_SOURCE ${CMAKE_CURRENT_SOURCE_DIR})

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/CMakeTools")

include(Options)
include(IncludeBoost)
include(OpenGL)

set(IMGUI_DIR ${CMAKE_CURRENT_LIST_DIR}/external/imgui)
list(APPEND INCLUDE_DIR_IMGUI ${IMGUI_DIR} ${IMGUI_DIR}/backends)
list(
    APPEND 
    IMGUI_SOURCE 
    ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
    ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_demo.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp 
    ${IMGUI_DIR}/imgui_tables.cpp
)

add_subdirectory(WinterSolstice-Core)
add_subdirectory(WinterSolstice-Editory)
# TODO: 如有需要，请添加测试并安装目标。
