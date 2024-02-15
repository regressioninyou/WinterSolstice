# CMakeList.txt: WinterSolstice 的 CMake 项目，在此处包括源代码并定义
# 项目特定的逻辑。
#
# 将源代码添加到此项目的可执行文件。
add_library(WinterSolstice-Core 
        "${CMAKE_CURRENT_LIST_DIR}/k423.cpp" 
        
        "${CMAKE_CURRENT_LIST_DIR}/Platform/load/stb_image/stb_image.cpp"

        "${CMAKE_CURRENT_LIST_DIR}/Platform/OpenGL/OpenGLContext.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/Platform/OpenGL/OpenGLBuffer.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/Platform/OpenGL/OpenGLVertexArray.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/Platform/OpenGL/OpenGLRendererAPI.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/Platform/OpenGL/OpenGLShader.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/Platform/OpenGL/OpenGLTexture.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/Platform/OpenGL/OpenGLFramebuffer.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/Platform/OpenGL/OpenGLUniformBuffer.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/Platform/OpenGL/OpenGLMeshShader.cpp"

        "${CMAKE_CURRENT_LIST_DIR}/Platform/Window/WindowsWindow.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/Platform/Window/WindowsInput.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/Platform/Window/WindowsPlatformUtils.cpp"
        
        "${CMAKE_CURRENT_LIST_DIR}/WinterSolstice/Animation/Animation.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/WinterSolstice/Animation/AnimationBone.cpp"

        "${CMAKE_CURRENT_LIST_DIR}/WinterSolstice/Core/Layer.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/WinterSolstice/Core/LayerStack.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/WinterSolstice/Core/Log.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/WinterSolstice/Core/Application.cpp" 
        "${CMAKE_CURRENT_LIST_DIR}/WinterSolstice/Core/StructBase.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/WinterSolstice/Core/True.cpp"

        "${CMAKE_CURRENT_LIST_DIR}/WinterSolstice/ImGui/ImGuiLayer.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/WinterSolstice/ImGui/ImCurveEdit.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/WinterSolstice/ImGui/ImGradient.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/WinterSolstice/ImGui/ImGuizmo.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/WinterSolstice/ImGui/ImSequencer.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/WinterSolstice/Light/Light.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/WinterSolstice/Math/Math.cpp"

        "${CMAKE_CURRENT_LIST_DIR}/WinterSolstice/Render/Shader.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/WinterSolstice/Render/Buffer.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/WinterSolstice/Render/Renderer.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/WinterSolstice/Render/Renderer2D.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/WinterSolstice/Render/RendererAPI.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/WinterSolstice/Render/VertexArray.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/WinterSolstice/Render/RendererCommand.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/WinterSolstice/Render/Framebuffer.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/WinterSolstice/Render/Texture.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/WinterSolstice/Render/UniformBuffer.cpp" 

        "${CMAKE_CURRENT_LIST_DIR}/WinterSolstice/Camera/OrthographicCamera.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/WinterSolstice/Camera/SceneCamera.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/WinterSolstice/Camera/OrthographicCameraController.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/WinterSolstice/Camera/EditorCamera.cpp"
        
        "${CMAKE_CURRENT_LIST_DIR}/WinterSolstice/Object/BVHImp.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/WinterSolstice/Object/Mesh.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/WinterSolstice/Object/BVH.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/WinterSolstice/Object/Loder.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/WinterSolstice/Object/Model.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/WinterSolstice/Object/Bounds3.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/WinterSolstice/Object/Triangle.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/WinterSolstice/Object/Object.cpp"

        "${CMAKE_CURRENT_LIST_DIR}/WinterSolstice/Scene/Scene.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/WinterSolstice/Scene/KdTree.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/WinterSolstice/Scene/Entity.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/WinterSolstice/Scene/SceneSerializer.cpp"
        
        "${IMGUI_SOURCE}"
)
target_compile_definitions(WinterSolstice-Core
PRIVATE
    -DPLATFORM_WINDOWS_S
    -DBUILD_DLL
    -DGLFW_INCLUDE_NONL
    -DIMGUI_IMPL_OPENGL_LOADER_GLAD
    -DENABLE_ASSERTS
    -DIMGUI_DEFINE_MATH_OPERATORS
)
target_precompile_headers(WinterSolstice-Core PRIVATE "k423.h")
# 或者对于多个头文件：
# target_precompile_headers(MyLibrary PRIVATE "path/to/header1.h" "path/to/header2.h")
target_sources(WinterSolstice-Core
PRIVATE 
PUBLIC  "${CMAKE_CURRENT_LIST_DIR}/WinterSolstice.h" 
        "${CMAKE_CURRENT_LIST_DIR}/k423.h"
        
        "${CMAKE_CURRENT_LIST_DIR}/Platform/load/stb_image/stb_image.h"
        "${CMAKE_CURRENT_LIST_DIR}/Platform/load/entt/entt.hpp"

        "${CMAKE_CURRENT_LIST_DIR}/Platform/OpenGL/OpenGLContext.h"
        "${CMAKE_CURRENT_LIST_DIR}/Platform/OpenGL/OpenGLBuffer.h"
        "${CMAKE_CURRENT_LIST_DIR}/Platform/OpenGL/OpenGLVertexArray.h"
        "${CMAKE_CURRENT_LIST_DIR}/Platform/OpenGL/OpenGLRendererAPI.h"
        "${CMAKE_CURRENT_LIST_DIR}/Platform/OpenGL/OpenGLShader.h"
        "${CMAKE_CURRENT_LIST_DIR}/Platform/OpenGL/OpenGLTexture.h"
        "${CMAKE_CURRENT_LIST_DIR}/Platform/OpenGL/OpenGLFramebuffer.h"
        "${CMAKE_CURRENT_LIST_DIR}/Platform/OpenGL/OpenGLUniformBuffer.h"
        "${CMAKE_CURRENT_LIST_DIR}/Platform/OpenGL/OpenGLMeshShader.h"

        "${CMAKE_CURRENT_LIST_DIR}/Platform/Window/WindowsWindow.h"
        "${CMAKE_CURRENT_LIST_DIR}/Platform/Window/WindowsInput.h"
        
        "${CMAKE_CURRENT_LIST_DIR}/WinterSolstice/Animation/Animation.h"
        "${CMAKE_CURRENT_LIST_DIR}/WinterSolstice/Animation/AnimationBone.h"
        "${CMAKE_CURRENT_LIST_DIR}/WinterSolstice/Animation/Bone.h"

        "${CMAKE_CURRENT_LIST_DIR}/WinterSolstice/Core/Input.h"
        "${CMAKE_CURRENT_LIST_DIR}/WinterSolstice/Core/KeyCodes.h"
        "${CMAKE_CURRENT_LIST_DIR}/WinterSolstice/Core/MouseCodes.h"
        "${CMAKE_CURRENT_LIST_DIR}/WinterSolstice/Core/Layer.h"
        "${CMAKE_CURRENT_LIST_DIR}/WinterSolstice/Core/LayerStack.h"
        "${CMAKE_CURRENT_LIST_DIR}/WinterSolstice/Core/Log.h"
        "${CMAKE_CURRENT_LIST_DIR}/WinterSolstice/Core/Application.h"
        "${CMAKE_CURRENT_LIST_DIR}/WinterSolstice/Core/Timestep.h"
        "${CMAKE_CURRENT_LIST_DIR}/WinterSolstice/Core/Time.h"
        "${CMAKE_CURRENT_LIST_DIR}/WinterSolstice/Core/StructBase.h"
        "${CMAKE_CURRENT_LIST_DIR}/WinterSolstice/Core/True.h"

        "${CMAKE_CURRENT_LIST_DIR}/WinterSolstice/ImGui/ImGuiLayer.h"
        "${CMAKE_CURRENT_LIST_DIR}/WinterSolstice/ImGui/ImCurveEdit.h"
        "${CMAKE_CURRENT_LIST_DIR}/WinterSolstice/ImGui/ImGradient.h"
        "${CMAKE_CURRENT_LIST_DIR}/WinterSolstice/ImGui/ImGuizmo.h"
        "${CMAKE_CURRENT_LIST_DIR}/WinterSolstice/ImGui/ImSequencer.h"

        
        "${CMAKE_CURRENT_LIST_DIR}/WinterSolstice/Light/Light.h"
        "${CMAKE_CURRENT_LIST_DIR}/WinterSolstice/Math/Math.h"
       

        "${CMAKE_CURRENT_LIST_DIR}/WinterSolstice/Events/ApplicationEvent.h"
        "${CMAKE_CURRENT_LIST_DIR}/WinterSolstice/Events/Event.h"
        "${CMAKE_CURRENT_LIST_DIR}/WinterSolstice/Events/KeyEvent.h"
        "${CMAKE_CURRENT_LIST_DIR}/WinterSolstice/Events/MouseEvent.h"

        "${CMAKE_CURRENT_LIST_DIR}/WinterSolstice/Render/GraphicsContext.h"
        "${CMAKE_CURRENT_LIST_DIR}/WinterSolstice/Render/Shader.h"
        "${CMAKE_CURRENT_LIST_DIR}/WinterSolstice/Render/Buffer.h"
        "${CMAKE_CURRENT_LIST_DIR}/WinterSolstice/Render/Renderer.h"
        "${CMAKE_CURRENT_LIST_DIR}/WinterSolstice/Render/Renderer2D.h"
        "${CMAKE_CURRENT_LIST_DIR}/WinterSolstice/Render/RendererAPI.h"
        "${CMAKE_CURRENT_LIST_DIR}/WinterSolstice/Render/VertexArray.h" 
        "${CMAKE_CURRENT_LIST_DIR}/WinterSolstice/Render/RendererCommand.h"
        "${CMAKE_CURRENT_LIST_DIR}/WinterSolstice/Render/Framebuffer.h"
        "${CMAKE_CURRENT_LIST_DIR}/WinterSolstice/Render/Texture.h" 
        "${CMAKE_CURRENT_LIST_DIR}/WinterSolstice/Render/UniformBuffer.h" 

        "${CMAKE_CURRENT_LIST_DIR}/WinterSolstice/Camera/OrthographicCamera.h"
        "${CMAKE_CURRENT_LIST_DIR}/WinterSolstice/Camera/OrthographicCameraController.h"
        "${CMAKE_CURRENT_LIST_DIR}/WinterSolstice/Camera/Camera.h"
        "${CMAKE_CURRENT_LIST_DIR}/WinterSolstice/Camera/SceneCamera.h"
        "${CMAKE_CURRENT_LIST_DIR}/WinterSolstice/Camera/EditorCamera.h"
        
        "${CMAKE_CURRENT_LIST_DIR}/WinterSolstice/Object/Mesh.h"
        "${CMAKE_CURRENT_LIST_DIR}/WinterSolstice/Object/BVH.h"
        "${CMAKE_CURRENT_LIST_DIR}/WinterSolstice/Object/BVHImp.h"
        "${CMAKE_CURRENT_LIST_DIR}/WinterSolstice/Object/Loder.h"
        "${CMAKE_CURRENT_LIST_DIR}/WinterSolstice/Object/Object.h"
        "${CMAKE_CURRENT_LIST_DIR}/WinterSolstice/Object/Bounds3.h"
        "${CMAKE_CURRENT_LIST_DIR}/WinterSolstice/Object/Model.h"
        "${CMAKE_CURRENT_LIST_DIR}/WinterSolstice/Object/Triangle.h"
        "${CMAKE_CURRENT_LIST_DIR}/WinterSolstice/Object/Ray.h"
        
        "${CMAKE_CURRENT_LIST_DIR}/WinterSolstice/Scene/Scene.h"
        "${CMAKE_CURRENT_LIST_DIR}/WinterSolstice/Scene/Entity.h"
        "${CMAKE_CURRENT_LIST_DIR}/WinterSolstice/Scene/KdTree.h"
        "${CMAKE_CURRENT_LIST_DIR}/WinterSolstice/Scene/Components.h"
        
        "${CMAKE_CURRENT_LIST_DIR}/WinterSolstice/Scene/ScritableEntity.h"
        "${CMAKE_CURRENT_LIST_DIR}/WinterSolstice/Scene/SceneSerializer.h"
        "${CMAKE_CURRENT_LIST_DIR}/WinterSolstice/Utils/PlatformUtils.h"
        "${CMAKE_CURRENT_LIST_DIR}/WinterSolstice/Utils/ThreadPool.h"
)

target_include_directories(WinterSolstice-Core
PUBLIC
"${CMAKE_CURRENT}"
"${INCLUDE_DIR}"
"${INCLUDE_DIR_IMGUI}"
)
#include(ImportLibrary)

find_package(glm CONFIG REQUIRED)
target_link_libraries(WinterSolstice-Core PRIVATE glm::glm)
find_package(spdlog CONFIG REQUIRED)
 target_link_libraries(WinterSolstice-Core PRIVATE spdlog::spdlog)
find_package(glad CONFIG REQUIRED)
target_link_libraries(WinterSolstice-Core PRIVATE glad::glad)
target_link_libraries(WinterSolstice-Core PRIVATE glfw)
target_link_libraries(WinterSolstice-Core PRIVATE ${Boost_LIBRARIES})

find_package(pugixml CONFIG REQUIRED)
target_link_libraries(WinterSolstice-Core PRIVATE pugixml::shared pugixml::pugixml)
find_package(ASSIMP CONFIG REQUIRED)
target_link_libraries(WinterSolstice-Core PRIVATE assimp::assimp)

find_package(unofficial-shaderc CONFIG REQUIRED)
target_link_libraries(WinterSolstice-Core PRIVATE unofficial::shaderc::shaderc)

find_package(spirv_cross_core CONFIG REQUIRED)
target_link_libraries(WinterSolstice-Core PRIVATE spirv-cross-core)

 find_package(spirv_cross_glsl CONFIG REQUIRED)
 target_link_libraries(WinterSolstice-Core PRIVATE spirv-cross-glsl)
 

set_target_properties(WinterSolstice-Core PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)