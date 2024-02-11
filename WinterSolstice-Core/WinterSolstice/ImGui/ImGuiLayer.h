#pragma once
#include "WinterSolstice/Core/Layer.h"
namespace WinterSolstice {
	namespace Hua {
		class ImGuiLayer : public WinterSolstice::Layer
		{
		public:
			ImGuiLayer();
			~ImGuiLayer();

			virtual void OnAttach() override;
			virtual void OnDetach()override;
			virtual void OnEvent(KnowTreasure::Event& e) override;

			void Begin();
			void End();

			void SetBlockFocosEvents(bool block) { m_BlockFocos = block; };
			bool GetBlockEvents() { return m_BlockFocos; };
			void SetDarkThemColors();

			uint32_t GetActiveWidgetID() const;
		private:
			bool m_BlockFocos = true;
			float m_Time = 0.0f;
		};
	}
}