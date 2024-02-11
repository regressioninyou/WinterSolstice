
#define PLATFORM_WINDOWS
#include <WinterSolstice.h>
#include "EditorLayout.h"
namespace WinterSolstice {
	class KyBaoEditor :public Application
	{
	public:
		KyBaoEditor():Application("KyBao-Editor") {
			PushLayer(new EditorLayout());
		};
		~KyBaoEditor() {};

	private:

	};

}
WinterSolstice::Application* WinterSolstice::CreateApplication() {
	return new WinterSolstice::KyBaoEditor();
}