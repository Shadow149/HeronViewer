#pragma once
#include "Module.h"
#include "Image.h"
#include "../common/Utils.h"

#include"imgui.h"
#include"imgui_impl_glfw.h"
#include"imgui_impl_opengl3.h"
#include <implot.h>

#define DATA_POINTS 255

class Curve : public Module
{
public:
	Curve(Image* i, std::string n, bool v = true) : Module(n, v) {
		img = i;
	};
	void init();
	void render();
	void cleanup();
private:
	const ImPlotAxisFlags flags = ImPlotAxisFlags_NoLabel | ImPlotAxisFlags_NoTickMarks | ImPlotAxisFlags_NoTickLabels | ImPlotAxisFlags_NoGridLines;
	const ImPlotFlags plotFlags = ImPlotFlags_NoTitle | ImPlotFlags_NoFrame | ImPlotFlags_NoBoxSelect | ImPlotFlags_NoMouseText | ImPlotFlags_NoInputs;
	Image* img;

	float x_data[DATA_POINTS];
	float y_data[4][DATA_POINTS];
	float line[2] = { 0,1 };
};

