#pragma once
#include "Module.h"
#include"imgui_impl_glfw.h"
#include"imgui_impl_opengl3.h"
#include"implot.h"
#include <vector>
#include <string>

//#define SHOW_FPS

enum
{
	FPS_POINTS = 255,
	MAX_STATS = 255
};

class Overlay :
	public Module
{
public:
	explicit Overlay(const std::string n, const bool v = false) : Module(n, v), fps_x_{}
	{
		pointer_ = 0;
		fps_pointer_ = 0;
	};
	void init() override;
	void render() override;
	void cleanup() override {};
	static std::string* register_metric();
	static void update_fps(float f);

private:
	static float fps_[FPS_POINTS];
	float fps_x_[FPS_POINTS];
	static int fps_pointer_;
	static std::string stats_[MAX_STATS];
	static int pointer_;
	ImGuiWindowFlags win_flags_ = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize |
		ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav;
};
