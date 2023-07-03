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
	Histogram(Image* i, const std::string n, const bool v = true) : Module(n, v) {
		img_ = i;
	}
	void init() override;
	void render() override;
	void cleanup() override {}
private:
	Image* img_;

	const ImPlotAxisFlags axis_flags_ = ImPlotAxisFlags_NoLabel | ImPlotAxisFlags_NoTickMarks | ImPlotAxisFlags_NoTickLabels | ImPlotAxisFlags_NoGridLines;
	const ImPlotFlags plot_flags_ = ImPlotFlags_NoTitle | ImPlotFlags_NoFrame | ImPlotFlags_NoBoxSelect | ImPlotFlags_NoMouseText | ImPlotFlags_NoLegend | ImPlotFlags_AntiAliased;
	
	unsigned hist_[4][256]{};
	unsigned hist_x_[256] = { 0 };
};

