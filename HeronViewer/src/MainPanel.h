#pragma once
#include "Module.h"
#include "Heron.h"
#include "Status.h"

#include"imgui.h"
#include"imgui_impl_glfw.h"
#include"imgui_impl_opengl3.h"
#include"ImGuiFileDialog.h"

class MainPanel : public Module
{
public:
	MainPanel(Window* h, std::string n, bool v = true) : Module(n, v, false) {
		hWindow = h;
	};
	void setStyle();
	void init();
	void render();
	void cleanup();
private:
	Window *hWindow;
};

