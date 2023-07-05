#include "Histogram.h"
#include "Image.h"

void Histogram::init()
{
	for (auto& n : hist_)
	{
		for (unsigned& i : n)
		{
			i = 0;
		}
	}
	for (int i = 0; i < 256; i++)
	{
		hist_x_[i] = i;
	}
}

void Histogram::render()
{
	if (!visible) { return; }

	for (auto& n : hist_)
	{
		for (unsigned& i : n)
		{
			i = 0;
		}
	}

	unsigned mean = 0;
	for (int n = 0; n < 4; n++)
	{
		for (int i = 0; i < 256; i++)
		{
			hist_[n][i] = img_->histogram[(i * 4) + n];
			mean += img_->histogram[(i * 4) + n];
		}
	}
	mean /= 256 / 2;

	ImGui::Begin(name.c_str(), &visible);
	const ImVec2 size = ImGui::GetWindowSize();
	ImPlot::GetStyle().PlotMinSize = ImVec2(0, 0);
	ImPlot::GetStyle().PlotDefaultSize = ImVec2(-1, size.y - 30);
	ImPlot::GetStyle().AntiAliasedLines = true;
	if (ImPlot::BeginPlot("Hist plot", ImVec2(-1, -1), plot_flags_))
	{
		ImPlot::SetupAxes("x", "y", axis_flags_, axis_flags_);
		// TODO ImPlotCond_Always doing unnecessary updates?
		ImPlot::SetupAxesLimits(0, 255, 0, mean, ImPlotCond_Always);
		for (int n = img_->is_bw() ? 0 : 3; n >= 0; n--)
		{
			ImPlot::SetNextFillStyle(channel_colors_[n]);
			ImPlot::PlotShaded(tab_names_[n], hist_x_, hist_[n], 256, -INFINITY);
		}
		ImPlot::EndPlot();
	}
	ImGui::End();
}
