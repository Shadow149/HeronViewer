#include "Overlay.h"

float Overlay::fps_[FPS_POINTS];
int Overlay::fps_pointer_;
std::string Overlay::stats_[MAX_STATS];
int Overlay::pointer_;

void Overlay::init()
{
	for (int i = 0; i < FPS_POINTS; i++)
	{
		fps_x_[i] = i;
	}
}

void Overlay::render()
{
	ImGui::SetNextWindowBgAlpha(0.35f);
	if (ImGui::Begin(name.c_str(), &visible, win_flags_))
	{
		for (int i = 0; i < pointer_; i ++)
		{
			ImGui::Text(stats_[i].c_str());
		}
#ifdef SHOW_FPS
        if (ImPlot::BeginPlot("FPS", ImVec2(-1, 0), plot_flags_)) {
			ImPlot::SetupAxes("x", "fps", axis_flags_, 0);
			ImPlot::SetupAxesLimits(0, FPS_POINTS, 0, 100);
            ImPlot::PlotLine("Curve", fps_x_, fps_, FPS_POINTS);
            ImPlot::EndPlot();
        }
#endif
	}
	ImGui::End();
}

std::string* Overlay::register_metric()
{
	if (pointer_ >= MAX_STATS)
		return nullptr;
	stats_[pointer_] = "";
	return &stats_[pointer_++];
}

void Overlay::update_fps(const float f)
{
	fps_[fps_pointer_++] = f;
	if (fps_pointer_ >= 255)
	{
		for (float& fp : fps_)
		{
			fp = 0;
		}
		fps_pointer_ = 0;
	}
}
