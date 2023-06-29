#include "Curve.h"

void Curve::init()
{
	for (int n = 0; n < 4; n++) {
		for (int i = 0; i < DATA_POINTS; i++) {
			float t = i / (float)DATA_POINTS;
			x_data[i] = t;
			y_data[n][i] = img->calcCurve(t, n);
		}
	}
	
}

void Curve::render()
{
	if (img->getChanged()) {
		for (int n = 0; n < 4; n++) {
			for (int i = 0; i < DATA_POINTS; i++) {
				float t = i / (float)DATA_POINTS;
				t = clamp(img->calcCurve(t, n), 0.01, 0.99);
				y_data[n][i] = t;
			}
		}
	}

	ImGui::SetNextWindowBgAlpha(0.50f); // Transparent background
	ImGui::Begin(name.c_str(), &visible);
	// TODO make this better
	ImVec2 size = ImGui::GetWindowSize();
	ImGui::SetWindowSize(ImVec2(size.x, size.x + 40));
	ImPlot::GetStyle().PlotMinSize = ImVec2(0, 0);
	ImPlot::GetStyle().PlotDefaultSize = ImVec2(-1, size.x);
	ImPlot::PushStyleColor(ImPlotCol_PlotBg, { 0,0,0,0 });
	if (ImPlot::BeginPlot("Curve", ImVec2(-1,-1), plotFlags)) {
		ImPlot::SetupAxes("x", "y", flags, flags);
		for (int n = 3; n >= 0; n--) {
			float weight = -1;
			if (n == 0)
				weight = 2;
			ImPlot::SetNextLineStyle(channel_colors[n], weight);
			ImPlot::PlotLine(tab_names[n], x_data, y_data[n], DATA_POINTS);
		}
		ImPlot::PlotLine("line", line, line, 2);
		ImPlot::EndPlot();
	}
	ImGui::End();
}

void Curve::cleanup()
{
}