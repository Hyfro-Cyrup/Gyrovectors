#include <cstdlib>
#include "SDL_main.h"

#include <App.h>
#include <windows.h>



int main(int argc, char* argv[]) {
	SetProcessDPIAware();
	App app;
	app.Run();


	return EXIT_SUCCESS;
}
