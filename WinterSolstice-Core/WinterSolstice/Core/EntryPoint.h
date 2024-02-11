#pragma once 
#ifdef PLATFORM_WINDOWS
extern  WinterSolstice::Application* WinterSolstice::CreateApplication();
int main(int argc, char** argv) {
	WinterSolstice::Log::Init();
	Kiana_CORE_INFO("hello");
	auto app = WinterSolstice::CreateApplication();
	app->Run();
	delete app;
	return 0;
}
#endif // PLATFROM_WINDOWS
