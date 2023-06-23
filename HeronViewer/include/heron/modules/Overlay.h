#pragma once
#include "Module.h"
#include"imgui_impl_glfw.h"
#include"imgui_impl_opengl3.h"
#include"implot.h"
#include <vector>
#include <string>

#define FPS_POINTS 255
#define MAX_STATS 255

class Overlay :
    public Module
{
private:
	static float fps[FPS_POINTS];
	float fps_x[FPS_POINTS];
	static int fps_pointer;
	static std::string stats[MAX_STATS];
	static int pointer;
	ImGuiWindowFlags flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav ; // | ImGuiWindowFlags_NoMove
public:
	Overlay(std::string n, bool v = false) : Module(n, v) { pointer = 0; fps_pointer = 0; };
	void init();
	void render();
	void cleanup();
	static std::string* registerMetric();
	static void updateFps(float f);
};

