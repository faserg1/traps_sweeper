#pragma once


namespace engine::render::opengl
{
	void checkError(int line = -1);
	bool hasError(int line = -1);
}