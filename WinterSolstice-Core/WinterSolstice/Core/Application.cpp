
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
		_RenderThread = new Himeko::SynchronizeThread(false);
		_TaskThread = new Himeko::SynchronizeThread();

		m_Window = std::unique_ptr<Window>(Window::Create(WindowProps(name)));
		m_Window->SetEventCallback(BIND_EVENT_FN(Application::OnEvent));

		Bronya::Renderer::Init();

		m_ImGuiLayer = new Hua::ImGuiLayer();
		PushOverlay(m_ImGuiLayer);

	}
	Application::~Application() {
		if (_TaskThread)
			delete _TaskThread;
		if (_RenderThread)
			delete _RenderThread;
	}

	void Application::Run() {
		//#define DEBUG
				//启动任务线程
		_TaskThread->FinalTaskBegin([this]() {
#ifdef DEBUG
			Kiana_CORE_INFO("_RenderThread SetExecute True");
#endif // DEBUG
			_RenderThread->AddTask([this]() {_RenderThread->SetExecute(true); });
			_RenderThread->FrameBegin();
			//_RenderThread->AddTask([this]() {_RenderThread->SetExecute(true); });
			_TaskThread->AddTask([this]() {
				float time = _RenderThread->AnsycTask([this]()->float {
					return (float)glfwGetTime();
					}).get();
					Kiana::Timestep timestep = time - m_LastFrameTime;
					this->m_LastFrameTime = time;
					if (!this->m_Minimized)
					{
						for (Layer* layer : this->m_LayerStack)
							//_TaskThread->AddTask([this, layer, timestep]() {layer->OnUpdate(timestep); });
							layer->OnUpdate(timestep);
					}
				});
			});

		_TaskThread->FinalTaskEnd([this]()
			{
				if (_RenderThread->GetExecute())
					_RenderThread->AddTask([this]() {_RenderThread->SetExecute(false); });
			});

		_RenderThread->FinalTaskEnd([this]()
			{
				if (!this->m_Minimized)
				{
					this->_RenderThread->AddTask([this]() {
						this->m_ImGuiLayer->Begin();

						for (Layer* layer : this->m_LayerStack)
							layer->OnImGuiRender();

						this->m_ImGuiLayer->End();
						});
					this->m_Window->OnUpdate();
				}
				else {
					this->m_Window->OnUpdate();
					this->_RenderThread->SetMinimized(this->m_Minimized);
				}
				if (!m_Running)
				{
					this->_TaskThread->Kill();
					this->_RenderThread->Kill();
				}
				else {
					_TaskThread->FrameBegin();
				}
			});

		_RenderThread->SetMinimizeRun([this]() {
			//if (!_RenderThread->GetMinimized()) _TaskThread->FrameBegin();
			//更新窗口
			this->m_Window->OnUpdate();
			if (!m_Running)
			{
				this->_TaskThread->Kill();
				this->_RenderThread->Kill();
			}
			else {
#ifdef DEBUG
				Kiana_CORE_INFO("_TaskThread FrameBegin");
#endif
				_TaskThread->AddMinimizedTask([this]() {
					_RenderThread->SetMinimized(this->m_Minimized);
					});
				_TaskThread->ExecuteWokerQueue();
				if (this->m_Minimized) std::this_thread::sleep_for(std::chrono::milliseconds(16));
			}
			});
		//需要事先激活一个线程
		_TaskThread->FrameBegin();
		_RenderThread->Run();
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
		//_RenderThreadPool->_RenderThreadWake();
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
		_RenderThread->AddTask([this, e]() {
			Bronya::Renderer::OnWindowResize(e.GetWidth(), e.GetHeight());
			});
		return false;
	}
}