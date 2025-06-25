#pragma once

#include "Utility\Includes.hpp"

// Dear ImGui: standalone example application for SDL3 + OpenGL
// (SDL is a cross-platform general purpose library for handling windows, inputs, OpenGL/Vulkan/Metal graphics context creation, etc.)

// Learn about Dear ImGui:
// - FAQ                  https://dearimgui.com/faq
// - Getting Started      https://dearimgui.com/getting-started
// - Documentation        https://dearimgui.com/docs (same as your local docs/ folder).
// - Introduction, links and more at the top of imgui.cpp

#include "imgui.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_opengl3.h"
#include <stdio.h>
#include <SDL3/SDL.h>
#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <SDL3/SDL_opengles2.h>
#else
#include <SDL3/SDL_opengl.h>
#endif

#ifdef __EMSCRIPTEN__
#include "../libs/emscripten/emscripten_mainloop_stub.h"
#endif

class C64;
class Debugger;

struct Context {
	SDL_Window *window;
	SDL_GLContext gl_context;
};

class GUI {
public:
	GUI(C64 *c64, Debugger *debug);
	~GUI() = default;
	GUI(GUI &&) = delete;
	GUI(const GUI &) = delete;
	GUI &operator=(GUI &&) = delete;
	GUI &operator=(const GUI &) = delete;

	i32 Init();
	void Render();
	void Destroy();

	void ShowMainMenuBar();
	void ShowDemoStuff();

	Context *GetContext() { return &mContext; };

private:
	C64 *mC64;
	Debugger *mDebug;
	
	Context mContext;

	LogPtr log = AddLogger("GUI");
};