#include "Waveform.h"

#include "Image.h"
#include "Widgets.h"

void Waveform::init()
{
}

void Waveform::render()
{
	ImGui::Begin(name.c_str(), &visible);
	ImGui::Image((ImTextureID)img->getWaveformID(), ImVec2(ImGui::GetWindowWidth(), ImGui::GetWindowHeight() - 60));
	ImGui::End();
}

void Waveform::cleanup()
{
}
