#include "Window.h"
#include <stdexcept>
#include "SDL.h"
#include "SDL_error.h"
#include "SDL_log.h"
#include "SDL_video.h"

Window::Window(int width, int height) : width{ width }, height{ height } {
	SDL_Setup();
	// make a window
	window = SDL_CreateWindow("This is the window title", 100, 100, width, height, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);

	// make an opengl context for that window
	context = SDL_GL_CreateContext(window);
}

void Window::SDL_Setup() {
	// Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO) != 0) {
		SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
		throw std::runtime_error("Unable to initialize SDL Video");
	}

	// no idea what this does
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

	// set the version
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);

	// double buffering and depth buffering
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

	// some kind of recommended parameter
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
}

void Window::Quit() {
	SDL_GL_DeleteContext(context);
	SDL_DestroyWindow(window);
	SDL_Quit();
}
