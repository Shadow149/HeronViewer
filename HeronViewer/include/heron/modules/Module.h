#pragma once
#include <string>
#include <utility>
#include"imgui.h"
#include"imgui_impl_glfw.h"
#include"imgui_impl_opengl3.h"

//#define SHOW_FPS


class Module
{
public:
	virtual ~Module() = default;

	Module(std::string n, const bool v, const bool s = true)
	{
		name = std::move(n);
		visible = v;
		show_module_option = s;
	}

	virtual void init() = 0;
	virtual void render() = 0;
	virtual void cleanup() = 0;
	virtual void set_style();
	void toggle_show() { visible = !visible; }
	void update_mouse_in_window();

public:
	std::string name;
	bool visible, show_module_option;

protected:
	bool mouse_in_window_{};
	const char* tab_names_[4] = {"Lum", "Red", "Green", "Blue"};
	ImVec4 channel_colors_[4] = {
		ImVec4(0.75, 0.75, 0.75, 1), ImVec4(1, 0, 0, 1), ImVec4(0, 1, 0, 1), ImVec4(0, 0, 1, 1)
	};
};
