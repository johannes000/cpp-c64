
#include "GUI.hpp"
#include "C64/C64.hpp"

// Main code
int main(int, char **) {

	C64 c64;

	GUI gui;
	auto ret = gui.Init();
	if (ret != 0)
		return ret;

	auto context = gui.GetContext();

	bool done = false;

	while (!done) {
		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			ImGui_ImplSDL3_ProcessEvent(&event);
			if (event.type == SDL_EVENT_QUIT)
				done = true;
			if (event.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED && event.window.windowID == SDL_GetWindowID(context->window))
				done = true;
		}

		if (SDL_GetWindowFlags(context->window) & SDL_WINDOW_MINIMIZED) {
			SDL_Delay(10);
			continue;
		}

		gui.Render(c64);
	}

	return 0;
}