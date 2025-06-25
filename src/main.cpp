
#include "C64/C64.hpp"
#include "C64/Debugger/Debugger.hpp"
#include "GUI.hpp"
#include "Utility/Logger.hpp"

auto main(int, char **) -> i32 {
	C64 c64;
	Debugger debug(&c64);
	GUI gui(&c64, &debug);

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
			if (event.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED &&
				event.window.windowID == SDL_GetWindowID(context->window))
				done = true;
		}

		if (SDL_GetWindowFlags(context->window) & SDL_WINDOW_MINIMIZED) {
			SDL_Delay(10);
			continue;
		}

		gui.Render();
	}

	gui.Destroy();
	spdlog::drop_all();

	return 0;
}