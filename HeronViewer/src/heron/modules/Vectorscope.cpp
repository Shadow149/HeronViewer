#include "Vectorscope.h"

#include "Image.h"
#include "Widgets.h"

void Vectorscope::render()
{
	ImGui::Begin(name.c_str(), &visible);
	hue_wheel(255 / 2, 255, 255, ImVec2(0, 0), 0, 30, true);
	ImGui::Image((ImTextureID)img_->get_vectorscope_id(), ImVec2(255, 255));
	ImGui::End();
}
