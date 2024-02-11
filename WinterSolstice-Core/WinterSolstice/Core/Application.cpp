
#include "k423.h"
#include "Application.h"
// KyBao.cpp: 定义应用程序的入口点。
//
#include "Input.h"
#include "Log.h"
#include "WinterSolstice/Render/Renderer.h"
#include <glfw/glfw3.h>
namespace WinterSolstice {

	Application* Application::s_Instance = nullptr;
	WinterSolstice::Himeko::ThreadPool Application::thp(4);
	std::pmr::unsynchronized_pool_resource Application::Memory_Pool;
	Application::Application(const std::string& name)
	{
		s_Instance = this;
		MainThread = new Himeko::BaseThread(false);

		m_Window = std::unique_ptr<Window>(Window::Create(WindowProps(name)));
		m_Window->SetEventCallback(BIND_EVENT_FN(Application::OnEvent));

		Bronya::Renderer::Init();

		m_ImGuiLayer = new Hua::ImGuiLayer();
		PushOverlay(m_ImGuiLayer);

	}
	Application::~Application() {
		if (MainThread)
			delete MainThread;
	}

	void Application::Run() {
		auto Update = [=, this]()
			{
				if (!m_Running)
				{
					this->MainThread->AddTask([this]() {this->MainThread->kill(); });
					return;
				}
				//std::lock_guard<std::mutex> lock(MainMutex);
				float time = (float)glfwGetTime();
				Kiana::Timestep timestep = time - m_LastFrameTime;
				this->m_LastFrameTime = time;

				if (!this->m_Minimized)
				{
#ifdef MoreAndMore
					this->MainThread->AddTask([this, timestep]() {
						for (Layer* layer : this->m_LayerStack)
							WaitWokers.emplace(
								std::move(
									this->GetThreadPool().AnsycTask([layer, timestep, this]()->int {
										layer->OnUpdate(timestep);
										return 1;
										})
								));
						});
					this->MainThread->AddTask([this, timestep]()
						{
							// 在这里等待所有异步任务完成
							while (!WaitWokers.empty()) {
								WaitWokers.front().get();
								WaitWokers.pop();
							}
						});
#else // MoreAndMore
					this->MainThread->AddTask([this, timestep]() {
						for (Layer* layer : this->m_LayerStack)
							layer->OnUpdate(timestep);
					});
#endif
					//for (Layer* layer : this->m_LayerStack)
						//this->MainThread->AddTask([layer, timestep, this]()->int {layer->OnUpdate(timestep); return 1; });
					//this->MainThread->AddTask([this]() {Bronya::Renderer::Execute(); });

					this->MainThread->AddTask([this, timestep]() {
						this->m_ImGuiLayer->Begin();

						for (Layer* layer : this->m_LayerStack)
							layer->OnImGuiRender();

						this->m_ImGuiLayer->End();
						});
					//this->MainThread->AddTask([this]() {this->m_ImGuiLayer->Begin(); });

					//for (Layer* layer : this->m_LayerStack)
						//this->MainThread->AddTask([layer, timestep, this]() {layer->OnImGuiRender(); });

					//this->MainThread->AddTask([this]() {this->m_ImGuiLayer->End(); });
				}
				this->MainThread->AddTask([this]() {this->m_Window->OnUpdate(); });
			};
		MainThread->AddTask(Update, Loop);
		MainThread->Worker();
#if 0
		while (m_Running)
		{
			//std::lock_guard<std::mutex> lock(MainMutex);
			float time = (float)glfwGetTime();
			Kiana::Timestep timestep = time - m_LastFrameTime;
			m_LastFrameTime = time;

			if (!m_Minimized)
			{
				for (Layer* layer : m_LayerStack)
					layer->OnUpdate(timestep);

				m_ImGuiLayer->Begin();

				for (Layer* layer : m_LayerStack)
					layer->OnImGuiRender();

				m_ImGuiLayer->End();
			}

			m_Window->OnUpdate();
		}
#endif // 0
		//MainThreadPool->MainThreadWake();
	}
	void  Application::OnEvent(KnowTreasure::Event& e)
	{
		KnowTreasure::EventDispatcher dispatcher(e);
		dispatcher.Dispatch<KnowTreasure::WindowCloseEvent>(BIND_EVENT_FN(Application::OnWindowClose));
		dispatcher.Dispatch<KnowTreasure::WindowResizeEvent>(BIND_EVENT_FN(Application::OnWindowResize));

		for (auto it = m_LayerStack.rbegin(); it != m_LayerStack.rend(); ++it)
		{
			if (e.Handled)
				break;
			(*it)->OnEvent(e);
		}
		}
	void Application::PushLayer(Layer* layer)
	{
		m_LayerStack.PushLayer(layer);
		layer->OnAttach();
	}
	void Application::PushOverlay(Layer* layer)
	{
		m_LayerStack.PushOverlay(layer);
		layer->OnAttach();
	}
	void Application::Close()
	{
		m_Running = false;
	}
	bool Application::OnWindowClose(KnowTreasure::WindowCloseEvent& e)
	{
		m_Running = false;
		return true;
	}
	bool Application::OnWindowResize(KnowTreasure::WindowResizeEvent& e)
	{
		if (e.GetWidth() == 0 || e.GetHeight() == 0)
		{
			m_Minimized = true;
			return false;
		}
		m_Minimized = false;
		Bronya::Renderer::OnWindowResize(e.GetWidth(), e.GetHeight());
		return false;
	}
	}