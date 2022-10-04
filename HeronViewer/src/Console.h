#pragma once
#include "Module.h"
#include"imgui.h"
#include"imgui_impl_glfw.h"
#include"imgui_impl_opengl3.h"
#include <string>
#include <vector>

#define MAX_LINES 255

// TODO make singleton
class Console : public Module
{
private:
public:
	static int line_pos;
	static std::string log_str;
	Console(std::string n, bool v = true) : Module(n, v) { line_pos = 0; };
	void init();
	void render();
	void cleanup();
	static void log(std::string log);
};

