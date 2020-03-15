#pragma once

#include "src/basic_types.h"

namespace engine::render::opengl
{
    void enableVertexAttribArray(u32 loc);
    void disableAllVertexAttribArray();
}