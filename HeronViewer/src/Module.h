#pragma once
#include <string>
#include"imgui.h"
#include"imgui_impl_glfw.h"
#include"imgui_impl_opengl3.h"

//#define SHOW_FPS


class Module
{
protected:
	bool mouseInWindow;
	const char* tab_names[4] = { "Lum","Red","Green","Blue" };
	ImVec4 channel_colors[4] = { ImVec4(1, 1, 1, 1), ImVec4(1, 0, 0, 1) , ImVec4(0, 1, 0, 1) , ImVec4(0, 0, 1, 1) };
public:
	Module(std::string n, bool v, bool s = true) { name = n; visible = v; showModuleOption = s; };
	std::string name;
	bool visible, showModuleOption;
	virtual void init() = 0;
	virtual void render() = 0;
	virtual void cleanup() = 0;
	void setStyle();
	void toggleShow() { visible = !visible; };
	void updateMouseInWindow();
};

