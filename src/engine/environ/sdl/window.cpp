#include "window.h"
#include <SDL.h>
#define GLEW_STATIC 
#include <GL/glew.h>
#include <iostream>
#include <stdexcept>


namespace engine::sdl
{
	struct Window::Impl
	{
		struct WinDeleter {
			void operator()(SDL_Window* ptr) {
				SDL_DestroyWindow(ptr);
			}
		};
		std::unique_ptr<SDL_Window, WinDeleter> window;
	};

	Window::Window(i32 width, i32 height) : _impl(std::make_unique<Impl>())
	{
		SDL_Init(SDL_INIT_VIDEO);
		#ifdef EMSCRIPTEN
		auto flags = SDL_WINDOW_OPENGL;
		#else
		auto flags = SDL_WINDOW_OPENGL | SDL_WINDOW_ALLOW_HIGHDPI;
		#endif
		
		#ifdef EMSCRIPTEN
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
		int major, minor;
		SDL_GL_GetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, &major);
		SDL_GL_GetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, &minor);
		
		std::cout << "The current version is " << major << "." << minor << ".\n";
		
		std::cout << "Set version 3.0" << std::endl;
		
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
		#else
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
		#endif
		
		_impl->window.reset(SDL_CreateWindow(
			"SDL2Test",
			SDL_WINDOWPOS_UNDEFINED,
			SDL_WINDOWPOS_UNDEFINED,
			width,
			height,
			flags
		));

		SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
		SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 32);

		auto gl = SDL_GL_CreateContext(_impl->window.get());
		if (!gl) {
			auto error = SDL_GetError();
			std::cerr << "OpenGL context creation failed. " << error << std::endl;
			throw std::runtime_error("OpenGL context creation failed.");
		}

		GLenum glewError = glewInit();
		if (glewError != GLEW_OK)
		{
			std::cerr << "Initializing GLEW failed. " << glewGetErrorString(glewError) << std::endl;
			throw std::runtime_error("Initializing GLEW failed.");
		}
		#ifdef EMSCRIPTEN
		// glEnable(GL_DEBUG_OUTPUT);
		// glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		/*glDebugMessageCallback( [](GLenum source,
                 GLenum type,
                 GLuint id,
                 GLenum severity,
                 GLsizei length,
                 const GLchar* message,
                 const void* userParam)
		{
			std::cout << message << "\n";
		}, 0 );*/
		#endif
		
		SDL_GL_GetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, &major);
		SDL_GL_GetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, &minor);
		
		std::cout << "The current version is " << major << "." << minor << ".\n";
	}
	Window::~Window() = default;
	void Window::swapBuffer()
	{
		SDL_GL_SwapWindow(_impl->window.get());
	}
}