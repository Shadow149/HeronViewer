#include "Histogram.h"
#include "Image.h"

void Histogram::init()
{
	int width = img->getWidth();
	int height = img->getHeight();
	unsigned char* data = img->getData();

	if (!data) {
		return;
	}
	for (int n = 0; n < 4; n++) {
		for (int i = 0; i < width * height * 4; i += 4) {
			int val = 0.2126 * data[i] + 0.7152 * data[i + 1] + 0.0722 * data[i + 2];
			hist[n][val] += 1;
		}
	}
	for (int i = 0; i < 256; i++) {
		hist_x[i] = i;
	}
}

void Histogram::render()
{
	if (!visible) { return; }

	if (img->getChanged()) {
		for (int n = 0; n < 4; n++) {
			for (int i = 0; i < 256; i++) {
				hist_edit[n][i] = 0;
			}
		}
		for (int n = 0; n < 4; n++) {
			for (int i = 0; i < 256; i++) {
				float t = (float)i / 255.0f;
				float r = 0, g = 0, b = 0;
				//t = img->calcCurve(t + 0.00001, n);
				img->calcCurveHist(t + 0.001, n, &r, &g, &b);

				r = clamp(r, 0.01f, 0.99f);
				g = clamp(g, 0.01f, 0.99f);
				b = clamp(b, 0.01f, 0.99f);

				int r_index = floor(r * 255.0f);
				int g_index = floor(g * 255.0f);
				int b_index = floor(b * 255.0f);

				hist_edit[1][r_index] += hist[n][i] / 3;
				hist_edit[2][g_index] += hist[n][i] / 3;
				hist_edit[3][b_index] += hist[n][i] / 3;
			}
		}
	}

	ImGui::Begin(name.c_str());
	ImVec2 size = ImGui::GetWindowSize();
	ImPlot::GetStyle().PlotMinSize = ImVec2(0, 0);
	ImPlot::GetStyle().PlotDefaultSize = ImVec2(-1, size.y - 30);
	ImPlot::GetStyle().AntiAliasedLines = true;
	//if (ImPlot::BeginPlot("Hist plot", ImVec2(-1,-1), plotFlags)) {
	if (ImPlot::BeginPlot("Hist plot", ImVec2(-1,-1))) {
		//ImPlot::SetupAxes("x", "y", flags, flags);
		ImPlot::SetupAxes("x", "y");
		for (int n = 3; n >= 1; n--) {
			ImPlot::SetNextLineStyle(channel_colors[n]);
			ImPlot::PlotLine(tab_names[n], hist_x, hist_edit[n], 256);
		}
		ImPlot::EndPlot();
	}
	ImGui::End();
}

void Histogram::cleanup()
{
}