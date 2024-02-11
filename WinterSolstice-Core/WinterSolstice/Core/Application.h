// KyBao.h: 标准系统包含文件的包含文件
// 或项目特定的包含文件。

#pragma once
#include "Core.h"
#include "WinterSolstice/Events/ApplicationEvent.h" 
#include "WinterSolstice/Core/Window.h"
#include "LayerStack.h"
#include "WinterSolstice/ImGui/ImGuiLayer.h"
#include "WinterSolstice/Render/Shader.h"
#include "WinterSolstice/Render/Buffer.h"
#include "WinterSolstice/Render/VertexArray.h"
#include <WinterSolstice/Utils/ThreadPool.h>
#include <memory_resource>
namespace WinterSolstice {
	class  Application
	{
	public:
		Application(const std::string& name);
		virtual ~Application();
		void Run();

		void OnEvent(KnowTreasure::Event& e);

		void PushLayer(Layer* layer);
		void PushOverlay(Layer* layer);

		void Close();

		Hua::ImGuiLayer* GetImGuiLayer() { return m_ImGuiLayer; };

		inline static Application& Get() { return *s_Instance; };

		inline Window& GetWindow() { return *m_Window; };
		inline glm::vec2 GetWindowPos() { return m_Window->GetWindowPos(); }

		Himeko::BaseThread*& GetMainThread() { return MainThread; }
		Himeko::BaseThread*& GetRendererThread() { return RendererThread; }

		static Himeko::ThreadPool& GetThreadPool() { return thp; }
		static std::pmr::unsynchronized_pool_resource Memory_Pool;
	private:
		bool OnWindowClose(KnowTreasure::WindowCloseEvent& e);
		bool OnWindowResize(KnowTreasure::WindowResizeEvent& e);
	private:
		Himeko::BaseThread* MainThread;
		Himeko::BaseThread* RendererThread;
		static Himeko::ThreadPool thp;
		Scope<Window> m_Window;
		Hua::ImGuiLayer* m_ImGuiLayer;
		bool m_Running = true;
		bool m_Minimized = false;
		LayerStack m_LayerStack;
		float m_LastFrameTime = 0.0f;

		std::queue<std::future<int>> WaitWokers;
	private:
		static Application* s_Instance;
	};

	Application* CreateApplication();
}

// TODO: 在此处引用程序需要的其他标头。
