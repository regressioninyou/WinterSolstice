#pragma once

#include "WinterSolstice/Events/Event.h"
#include "WinterSolstice/Core/Timestep.h"

namespace WinterSolstice {

	class Layer
	{
	public:
		Layer(const std::string& name = "Layer");
		virtual ~Layer() = default;

		virtual void OnAttach() {}
		virtual void OnDetach() {}
		virtual void OnUpdate(Kiana::Timestep ts) {}
		virtual void OnImGuiRender() {};
		virtual void OnEvent(KnowTreasure::Event& event) {}

		const std::string& GetName() const { return m_DebugName; }
	protected:
		std::string m_DebugName;
	};

}