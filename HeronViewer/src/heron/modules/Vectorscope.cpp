#include "Vectorscope.h"

#include "Image.h"
#include "Widgets.h"
#include "Utils.h"

void Vectorscope::render()
{
	ImGui::Begin(name.c_str());
	float width = min(ImGui::GetWindowWidth() - 30, ImGui::GetWindowHeight() - 30);
	hue_wheel(width / 2, width, width, ImVec2(0, 0), 0, 30, true);
	ImGui::Image((ImTextureID)img_->get_vectorscope_id(), ImVec2(width,width));
	ImGui::End();
}
