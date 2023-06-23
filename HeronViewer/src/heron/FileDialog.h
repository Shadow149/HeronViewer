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
	FileDialog(Heron* h, std::string n, bool v = true) : Module(n, v, false) { hWindow = h; };
	void init();
	void render();
	void cleanup();
	void setStyle();
	const char* fileName;
	std::string escape;
private:
	Heron* hWindow;
};

