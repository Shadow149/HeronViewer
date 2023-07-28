#pragma once
#include "Module.h"
#include"imgui.h"
#include"imgui_impl_glfw.h"
#include"imgui_impl_opengl3.h"
#include <string>
#include <vector>

enum
{
	MAX_LINES = 512,
	BUFFER_MAX = 512,
	MAX_LOG_SIZE = MAX_LINES * BUFFER_MAX
};

// TODO make singleton
class Console : public Module
{
public:
	explicit Console(const std::string& n, const bool v = true) : Module(n, v), font_(nullptr)
	{
		line_pos_ = 0;
	}
	void init() override;
	void render() override;
	void cleanup() override;
	static void log(const char* format, ...);

private:
	ImFont* font_;

	static int line_pos_;
	static char log_str_[MAX_LOG_SIZE];
};
