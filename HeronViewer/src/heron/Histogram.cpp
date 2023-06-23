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
		for (int i = 0; i < 256; i++) {
			hist[n][i] = 0;
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

		int width = img->getWidth();
		int height = img->getHeight();
		if (height == 1 || !img->histogram_loaded) return;
		unsigned char* data = img->getData();
		for (int n = 0; n < 4; n++) {
			for (int i = 0; i < 256; i++) {
				hist[n][i] = 0;
			}
		}
		for(int i = 0; i < 256; i ++)
		{
			hist[0][i] = img->histogram[(i*4)];
			hist[1][i] = img->histogram[(i*4)+1];
			hist[2][i] = img->histogram[(i*4)+2];
			hist[3][i] = img->histogram[(i*4)+3];
		}
	}
	ImGui::Begin(name.c_str());
	ImVec2 size = ImGui::GetWindowSize();
	ImPlot::GetStyle().PlotMinSize = ImVec2(0, 0);
	ImPlot::GetStyle().PlotDefaultSize = ImVec2(-1, size.y - 30);
	ImPlot::GetStyle().AntiAliasedLines = true;
	if (ImPlot::BeginPlot("Hist plot", ImVec2(-1,-1), plotFlags)) {
		ImPlot::SetupAxes("x", "y", flags, flags);
		double foo = (double)(img->getHeight() * img->getWidth()) / 20.0;
		// TODO ImPlotCond_Always doing unnecessary updates?
		ImPlot::SetupAxesLimits(0, 255, 0, foo, ImPlotCond_Always);
		for (int n = 3; n >= 0; n--) {
			ImPlot::PushStyleVar(ImPlotStyleVar_FillAlpha, 0.5f);
			ImPlot::SetNextFillStyle(channel_colors[n]);
			ImPlot::PlotShaded(tab_names[n], hist_x, hist[n], 256, -INFINITY);
			ImPlot::PopStyleVar();
		}
		ImPlot::EndPlot();
	}
	ImGui::End();
}

void Histogram::cleanup()
{
}