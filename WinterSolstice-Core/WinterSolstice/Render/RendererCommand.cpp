#include "k423.h"
#include "RendererCommand.h"
#include "Platform/OpenGL/OpenGLRendererAPI.h"

namespace WinterSolstice {
	namespace Bronya {
		RendererAPI* RenderCommand::s_RendererAPI = new OpenGLRendererAPI();
	}
}