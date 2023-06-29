#include "Overlay.h"

float Overlay::fps[FPS_POINTS];
int Overlay::fps_pointer;
std::string Overlay::stats[MAX_STATS];
int Overlay::pointer;

void Overlay::init()
{
    for (int i = 0; i < FPS_POINTS; i++) {
        fps_x[i] = i;
    }
}

void Overlay::render()
{
    ImGui::SetNextWindowBgAlpha(0.35f);
    if (ImGui::Begin(name.c_str(), &visible, flags))
    {
        for (int i = 0; i < pointer; i ++) {
            ImGui::Text(stats[i].c_str());
        }
#ifdef SHOW_FPS
        if (ImPlot::BeginPlot("FPS")) {
            ImPlot::PlotLine("Curve", fps_x, fps, FPS_POINTS);
            ImPlot::EndPlot();
        }
#endif
    }
    ImGui::End();
}

void Overlay::cleanup()
{
}

std::string* Overlay::registerMetric() {
    if (pointer >= MAX_STATS)
        return nullptr;
    stats[pointer] = "";
    return &stats[pointer++];
}

void Overlay::updateFps(float f) {
    fps[fps_pointer++] = f;
    if (fps_pointer >= 255) {
        for (int i = 0; i < FPS_POINTS; i++) {
            fps[i] = 0;
        }
        fps_pointer = 0;
    }
}