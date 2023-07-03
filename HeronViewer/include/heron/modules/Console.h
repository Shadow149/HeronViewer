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
public:
	static int line_pos;
	static std::string log_str;
	explicit Console(const std::string& n, const bool v = true) : Module(n, v), font_(nullptr)
	{
		line_pos = 0;
	}
	void init() override;
	void render() override;
	void cleanup() override;
	static void log(const std::string& log);

private:
	ImFont* font_;
};
