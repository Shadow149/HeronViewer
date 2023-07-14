#include "Waveform.h"

#include "Image.h"
#include "Widgets.h"

void Waveform::render()
{
	ImGui::Begin(name.c_str());
	ImGui::Image((ImTextureID)img_->get_waveform_id(), ImVec2(ImGui::GetWindowWidth(), ImGui::GetWindowHeight() - 60));
	ImGui::End();
}
