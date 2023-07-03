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
	MainPanel(Window* h, const std::string n, const bool v = true) : Module(n, v, false)
	{
		heron_window_ = h;
	}
	void init() override;
	void render() override;
	void cleanup() override;

private:
	Window* heron_window_;
};
