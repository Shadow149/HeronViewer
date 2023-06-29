#pragma once
#include "Module.h"
#include "../common/Utils.h"

#include"imgui.h"
#include"imgui_impl_glfw.h"
#include"imgui_impl_opengl3.h"
#include <implot.h>

class Image;

class Histogram : public Module
{
public:
	Histogram(Image* i, std::string n, bool v = true) : Module(n, v) {
		img = i;
	};
	void init();
	void render();
	void cleanup();
private:
	Image* img;
	//
	const ImPlotAxisFlags flags = ImPlotAxisFlags_NoLabel | ImPlotAxisFlags_NoTickMarks | ImPlotAxisFlags_NoTickLabels | ImPlotAxisFlags_NoGridLines;
	// ImPlotFlags_NoInputs
	const ImPlotFlags plotFlags = ImPlotFlags_NoTitle | ImPlotFlags_NoFrame | ImPlotFlags_NoBoxSelect | ImPlotFlags_NoMouseText | ImPlotFlags_NoLegend | ImPlotFlags_AntiAliased;
	
	int hist[4][256];
	int hist_edit[4][256];
	int hist_x[256] = { 0 };
};

