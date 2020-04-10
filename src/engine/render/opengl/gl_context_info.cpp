#define GLEW_STATIC 
#include <GL/glew.h>
#include <iostream>
#include <string>

#include "error_handling.h"

namespace engine::render::opengl
{
    int getMajorVersion()
    {
        std::string_view ver{(char *) glGetString(GL_VERSION)};
        if (ver.find_first_of("ES 2.") == std::string_view::npos)
            return 3;
        return 2;
    }
}