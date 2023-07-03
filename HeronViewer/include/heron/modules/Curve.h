#pragma once
#include "Module.h"
#include "Image.h"
#include "../common/Utils.h"

#include"imgui.h"
#include"imgui_impl_glfw.h"
#include"imgui_impl_opengl3.h"
#include <implot.h>

enum
{
	DATA_POINTS = 255
};

class Curve final : public Module
{
public:
	Curve(Image* i, const std::string& n, const bool v = true) : Module(n, v) {
		img_ = i;
	}
	void init() override;
	void render() override;
	void cleanup() override;
private:
	const ImPlotAxisFlags axis_flags_ = ImPlotAxisFlags_NoLabel | ImPlotAxisFlags_NoTickMarks | ImPlotAxisFlags_NoTickLabels | ImPlotAxisFlags_NoGridLines;
	const ImPlotFlags plot_flags_ = ImPlotFlags_NoTitle | ImPlotFlags_NoFrame | ImPlotFlags_NoBoxSelect | ImPlotFlags_NoMouseText | ImPlotFlags_NoInputs | ImPlotFlags_NoLegend;
	Image* img_;

	float x_data_[DATA_POINTS]{};
	float y_data_[4][DATA_POINTS]{};
	float line_[2] = { 0,1 };
};

