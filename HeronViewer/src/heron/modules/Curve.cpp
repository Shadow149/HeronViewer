#include "Curve.h"

void Curve::init()
{
	for (int n = 0; n < 4; n++) {
		for (int i = 0; i < DATA_POINTS; i++) {
			const float t = i / (float)DATA_POINTS;
			x_data_[i] = t;
			y_data_[n][i] = img_->calc_curve(t, n);
		}
	}
	
}

void Curve::render()
{
	if (img_->get_changed()) {
		for (int n = 0; n < 4; n++) {
			for (int i = 0; i < DATA_POINTS; i++) {
				float t = i / (float)DATA_POINTS;
				t = clamp(img_->calc_curve(t, n), 0.01, 0.99);
				y_data_[n][i] = t;
			}
		}
	}

	ImGui::SetNextWindowBgAlpha(0.50f); // Transparent background
	ImGui::Begin(name.c_str());
	// TODO make this better
	const ImVec2 size = ImGui::GetWindowSize();
	ImGui::SetWindowSize(ImVec2(size.x, size.x + 40));
	ImPlot::GetStyle().PlotMinSize = ImVec2(0, 0);
	ImPlot::GetStyle().PlotDefaultSize = ImVec2(-1, size.x);
	ImPlot::PushStyleColor(ImPlotCol_PlotBg, { 0,0,0,0 });
	if (ImPlot::BeginPlot("Curve", ImVec2(-1,-1), plot_flags_)) {
		ImPlot::SetupAxes("x", "y", axis_flags_, axis_flags_);
		for (int n = 3; n >= 0; n--) {
			float weight = -1;
			if (n == 0)
				weight = 2;
			ImPlot::SetNextLineStyle(channel_colors_[n], weight);
			ImPlot::PlotLine(tab_names_[n], x_data_, y_data_[n], DATA_POINTS);
		}
		ImPlot::PlotLine("line", line_, line_, 2);
		ImPlot::EndPlot();
	}
	ImGui::End();
}

void Curve::cleanup()
{
}