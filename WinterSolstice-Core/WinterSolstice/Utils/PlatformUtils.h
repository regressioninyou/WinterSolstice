#pragma once 
#include <string>
namespace WinterSolstice {
	class FileDialogs
	{
	public:
		static std::string OpenFile(const char* fileter);
		static std::string SaveFile(const char* fileter);
	};
}