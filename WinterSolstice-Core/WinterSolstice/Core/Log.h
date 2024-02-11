#pragma once
#pragma warning(disable: 26495)
#pragma warning(disable: 26498)
#include "Core.h"
#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>
namespace WinterSolstice {
	class Log
	{
	public:
		static void Init();

		static std::shared_ptr<spdlog::logger>& GetCoreLogger() { return s_CoreLogger; }
		static std::shared_ptr<spdlog::logger>& GetClientLogger() { return s_ClientLogger; }
	private:
		static std::shared_ptr<spdlog::logger> s_CoreLogger;
		static std::shared_ptr<spdlog::logger> s_ClientLogger;
	};
}
// Core log macros
#define Kiana_CORE_TRACE(...)    ::WinterSolstice::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define Kiana_CORE_INFO(...)     ::WinterSolstice::Log::GetCoreLogger()->info(__VA_ARGS__)
#define Kiana_CORE_WARN(...)     ::WinterSolstice::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define Kiana_CORE_ERROR(...)    ::WinterSolstice::Log::GetCoreLogger()->error(__VA_ARGS__)
#define Kiana_CORE_CRITICAL(...) ::WinterSolstice::Log::GetCoreLogger()->critical(__VA_ARGS__)

// Client log macros
#define Kiana_TRACE(...)         ::WinterSolstice::Log::GetClientLogger()->trace(__VA_ARGS__)
#define Kiana_INFO(...)          ::WinterSolstice::Log::GetClientLogger()->info(__VA_ARGS__)
#define Kiana_WARN(...)          ::WinterSolstice::Log::GetClientLogger()->warn(__VA_ARGS__)
#define Kiana_ERROR(...)         ::WinterSolstice::Log::GetClientLogger()->error(__VA_ARGS__)
#define Kiana_CRITICAL(...)      ::WinterSolstice::Log::GetClientLogger()->critical(__VA_ARGS__)
