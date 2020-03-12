#include "error_handling.h"

#define GLEW_STATIC 
#include <GL/glew.h>
#include <iostream>

namespace engine::render::opengl
{
	void checkError(int line) {
		if (hasError(line))
			throw;
	}

	bool hasError(int line) {
		#ifdef NDEBUG
		#else 
		auto err = glGetError();
		if (err) {
			std::cerr << "GL error code: 0x" << std::hex << err << std::dec;
			if (line >= 0)
				std::cerr << " on line " << line;
			std::cerr << std::endl;
			return true;
		}
		#endif
		return false;
	}
}