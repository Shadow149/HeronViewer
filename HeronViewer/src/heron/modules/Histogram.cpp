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
	
	ImGui::Begin(name.c_str(), &visible);
	const ImVec2 size = ImGui::GetWindowSize();
	ImPlot::GetStyle().PlotMinSize = ImVec2(0, 0);
	ImPlot::GetStyle().PlotDefaultSize = ImVec2(-1, size.y - 30);
	ImPlot::GetStyle().AntiAliasedLines = true;
	if (ImPlot::BeginPlot("Hist plot", ImVec2(-1,-1), plotFlags)) {
		ImPlot::SetupAxes("x", "y", flags, flags);
		const double y_max = static_cast<double>(img->getHeight() * img->getWidth()) / 20.0 / 2.0;
		// TODO ImPlotCond_Always doing unnecessary updates?
		ImPlot::SetupAxesLimits(0, 255, 0, y_max, ImPlotCond_Always);
		for (int n = img->isBW() ? 0 : 3; n >= 0; n--) {
			ImPlot::SetNextFillStyle(channel_colors[n]);
			ImPlot::PlotShaded(tab_names[n], hist_x, hist[n], 256, -INFINITY);
		}
		ImPlot::EndPlot();
	}
	ImGui::End();
}

void Histogram::cleanup()
{
}