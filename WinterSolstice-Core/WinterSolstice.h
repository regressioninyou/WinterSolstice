#pragma once
#define GLFW_INCLUDE_NONE  // 防止GLFW包含OpenGL头文件
#include "imgui.h"
#include "WinterSolstice/Core/Application.h"
#include "WinterSolstice/Core/Layer.h"
#include "WinterSolstice/Core/Log.h"

#include "WinterSolstice/Core/Timestep.h"

#include "WinterSolstice/Core/Input.h"
#include "WinterSolstice/Core/KeyCodes.h"
#include "WinterSolstice/Events/KeyEvent.h"
#include "WinterSolstice/Core/MouseCodes.h"
#include "WinterSolstice/Camera/OrthographicCameraController.h"

#include "WinterSolstice/ImGui/ImGuiLayer.h"

#include "WinterSolstice/Object/Object.h"
#include "WinterSolstice/Object/BVH.h"
#include "WinterSolstice/Object/Loder.h"
#include "WinterSolstice/Object/Model.h"
#include "WinterSolstice/Object/Ray.h"
#include "WinterSolstice/Object/Mesh.h"

#include "WinterSolstice/Scene/Scene.h"
#include "WinterSolstice/Scene/Entity.h"
#include "WinterSolstice/Scene/ScritableEntity.h"
#include "WinterSolstice/Scene/Components.h"
#include "WinterSolstice/Scene/SceneSerializer.h"
//-------Renderer----------------------
#include "WinterSolstice/Render/Renderer.h"
#include "WinterSolstice/Render/Renderer2D.h"
#include "WinterSolstice/Render/RendererCommand.h"

#include "WinterSolstice/Render/Buffer.h"
#include "WinterSolstice/Render/Shader.h"
#include "WinterSolstice/Render/Framebuffer.h"
#include "WinterSolstice/Render/Texture.h"
#include "WinterSolstice/Render/VertexArray.h"

#include "WinterSolstice/Camera/OrthographicCamera.h"

#include "WinterSolstice/Core/EntryPoint.h"