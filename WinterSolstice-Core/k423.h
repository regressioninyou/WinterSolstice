#pragma once 
#pragma warning(disable: 4996)
#pragma warning(disable: 26498)
#include <iostream>
#include <memory>
#include <utility>
#include <algorithm>
#include <functional>

#include <string>
#include <sstream>
#include <array>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <glm/glm.hpp>


#include "WinterSolstice/Core/Core.h"
#include "WinterSolstice/Core/Log.h"
//#define MoreAndMore
#ifdef PLATFORM_WINDOWS_S
	#define NOMINMAX
	#include <Windows.h>
#endif // PLATFORM_WINDOWS
