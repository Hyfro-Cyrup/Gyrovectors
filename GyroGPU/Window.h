#pragma once
#include "SDL_video.h"
#include <iostream>


class Window {
private: 
	int width, height;
	SDL_Window* window;
	SDL_GLContext context;
public:
	Window(int width, int height);

	~Window() noexcept {
		Quit();
	}

	void SwapBuffers() {
		SDL_GL_SwapWindow(window);
	}

	void GetDrawDimensions(int* w_out, int* h_out) {
		SDL_GL_GetDrawableSize(window, w_out, h_out);
	}

private:
	void SDL_Setup();

	void Quit();
};