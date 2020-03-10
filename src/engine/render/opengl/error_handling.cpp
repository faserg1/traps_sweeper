#include "error_handling.h"

#define GLEW_STATIC 
#include <GL/glew.h>
#include <iostream>

namespace engine::render::opengl
{
	void checkError(int line) {
#ifdef NDEBUG
#else 
	auto err = glGetError();
	if (err) {
		std::cerr << "GL error code: 0x" << std::hex << err << std::dec << " on line " << line << std::endl;
		throw;
	}
#endif

	}
}