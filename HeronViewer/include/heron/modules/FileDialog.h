#pragma once
#include "Module.h"
#include "Console.h"

#include"imgui.h"
#include"imgui_impl_glfw.h"
#include"imgui_impl_opengl3.h"
#include"ImGuiFileDialog.h"

class Heron;

class FileDialog : public Module
{
public:
	FileDialog(Heron* h, const std::string n, const bool v = true) : Module(n, v, false) { heron_window_ = h; }
	void init() override {};
	void render() override;
	void cleanup() override {};
	void set_style() override;

private:
	Heron* heron_window_;
	std::string escape_;
};
