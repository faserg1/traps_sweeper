#include "gl_enabled_attributes.h"
#include <unordered_set>

#define GLEW_STATIC 
#include <GL/glew.h>

namespace
{
    // enabled attributes are global anyway. So why not to store ig globally?
    std::unordered_set<GLuint> vertexAttribs;
}

namespace engine::render::opengl
{
    void enableVertexAttribArray(u32 loc)
    {
        glEnableVertexAttribArray(loc);
        vertexAttribs.insert(loc);
    }

    void disableAllVertexAttribArray()
    {
        for (auto loc: vertexAttribs) {
            glDisableVertexAttribArray(loc);
        }
        vertexAttribs.clear();
    }
}
